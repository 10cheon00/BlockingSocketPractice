#include "Packet.h"
#include "Constants.h"

namespace PNet{
	Packet::Packet(PacketType packetType){
		Clear();
		AssignPacketType(packetType);
	}

	PacketType Packet::GetPacketType(){
		PacketType* packetTypePtr = reinterpret_cast<PacketType*>(&buffer[0]);
		return static_cast<PacketType>(ntohs(*packetTypePtr));
	}

	void Packet::AssignPacketType(PacketType packetType){
		PacketType* packetTypePtr = reinterpret_cast<PacketType*>(&buffer[0]);
		*packetTypePtr = static_cast<PacketType>(htons(packetType));
	}

	//Set buffer size same as PacketType, variable of extractionOffset to size of PacketType.
	//Now Managing data position is after 
	void Packet::Clear(){
		buffer.resize(sizeof(PacketType));
		AssignPacketType(PacketType::PT_Invalid);
		extractionOffset = sizeof(PacketType);
	}

	void Packet::Append(const void* data, uint32_t size){
		if((buffer.size() + size) > PNet::g_MaxPacketSize)
			throw PacketException("[Packet::Append(const void*,uint32_t)] Packet size exceeded max packet size.");

		buffer.insert(buffer.end(), (char*)data, (char*)data + size);
	}

	//Just append data.
	Packet& Packet::operator<<(uint32_t data){
		data = htonl(data);
		Append(&data, sizeof(uint32_t));
		return *this;
	}

	//1. Attempt to extract data beyond buffer size, thrown exception.
	//2. Extract data at extractionOffset position.
	//3. Add size of uint32_t structure to extractionOffset.
	Packet& Packet::operator>>(uint32_t& data){
		if((extractionOffset + sizeof(uint32_t)) > buffer.size())
			throw PacketException("[Packet::operator>>(uint32_t&)] Extraction offset exceeded buffer size.");

		//Cast buffer to uint32_t* structure.
		//Get it's pointer and put it into parameter.
		data = *reinterpret_cast<uint32_t*>(&buffer[extractionOffset]);
		data = ntohl(data);

		//Add size of uint32_t to Offset.
		//Next time, extraction will begin at location that variable is pointing.
		extractionOffset += sizeof(uint32_t);
		return *this;
	}

	//1. Append data length first.
	//2. Append data which length must be same what previoulsy appended.
	Packet& Packet::operator<<(const std::string& data){
		*this << (uint32_t)data.size();
		Append(data.data(), data.size());
		return *this;
	}

	//1. Extract data length first.
	//2. Resize data with previously extracted length value.
	//3. Extract data and add its length to offset.
	Packet& Packet::operator>>(std::string& data){
		data.clear();

		//Get data length
		uint32_t stringSize = 0;
		*this >> stringSize;

		//
		if((extractionOffset + stringSize) > buffer.size())
			throw PacketException("[Packet::operator>>(std::string&)] Extraction offset exceeded buffer size.");

		//Resize parameter with stringSize and store data.
		data.resize(stringSize);
		data.assign(&buffer[extractionOffset], stringSize);
		//Add sizeof stringSize to Offset.
		extractionOffset += stringSize;
		return *this;
	}
}