#pragma once
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <WinSock2.h>
#include "PacketException.h"
#include "PacketType.h"

namespace PNet{
	/**	
	 *	Structure of Buffer.
	 *	[PacketType][Data][Data][Data] ... 
	 *	
	 *	Types of Data
	 *	integers	
	 *	string		
	 */
	class Packet{
	public:
		Packet(PacketType packetType = PacketType::PT_Invalid);
		PacketType GetPacketType();
		void AssignPacketType(PacketType packetType);

		void Clear();
		void Append(const void* data, uint32_t size);
		
		//Push data into Packet 
		Packet& operator << (uint32_t data);
		Packet& operator << (const std::string& data);
		
		//Pop data from Packet
		Packet& operator >> (uint32_t& data);
		Packet& operator >> (std::string& data);

		//When trying to use extract operator, this variable is where to starting point of extraction.
		uint32_t extractionOffset = 0;
		std::vector<char> buffer;
	};
}