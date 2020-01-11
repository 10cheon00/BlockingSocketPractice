#pragma once
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <WinSock2.h>
#include <string>
namespace PNet{
	class Packet{
	public:
		void Clear();
		void Append(const void* data, uint32_t size);
		
		Packet& operator << (uint32_t data);
		Packet& operator >> (uint32_t& data);

		Packet& operator << (const std::string& data);
		Packet& operator >> (std::string& data);

		//When trying to use extract operator, this variable is where to starting point of extraction.
		uint32_t extractionOffset = 0;
		std::vector<char> buffer;
	};
}