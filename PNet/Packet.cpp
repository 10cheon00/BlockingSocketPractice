#include "Packet.h"

namespace PNet{
	void Packet::Clear(){
		buffer.clear();
		extractionOffset = 0;
	}

	void Packet::Append(const void* data, uint32_t size){
		buffer.insert(buffer.end(), (char*)data, (char*)data + size);
	}

	Packet& Packet::operator<<(uint32_t data){
		data = htonl(data);
		Append(&data, sizeof(uint32_t));
		return *this;
	}

	Packet& Packet::operator>>(uint32_t& data){
		//Cast buffer to uint32_t* structure.
		//Get it's pointer and put it into parameter.
		data = *reinterpret_cast<uint32_t*>(&buffer[extractionOffset]);
		data = ntohl(data);

		//Add sizeof uint32_t to Offset.
		//Next time, extraction will begin at location that variable is pointing.
		extractionOffset += sizeof(uint32_t);
		return *this;
	}

	Packet& Packet::operator<<(const std::string& data){
		//Append data length
		*this << (uint32_t)data.size();
		//Append data
		Append(data.data(), data.size());
		return *this;
	}

	Packet& Packet::operator>>(std::string& data){
		data.clear();
		uint32_t stringSize = 0;

		//Get data length
		*this >> stringSize;

		//Resize parameter with stringSize and store data.
		data.resize(stringSize);
		data.assign(&buffer[extractionOffset], stringSize);
		//Add sizeof stringSize to Offset.
		extractionOffset += stringSize;
		return *this;
	}
}