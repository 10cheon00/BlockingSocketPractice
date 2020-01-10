#include "IPEndpoint.h"
#include <assert.h>
#include <iostream>
namespace PNet{
	//ntohs		return value is host byte order(=little endian)
	//htons		return value is network byte order(=big endian)

	IPEndpoint::IPEndpoint(const char* ip, unsigned short port){
		this->port = port;
		
		in_addr addr; //location to store the ipv4 address
		int result = inet_pton(
			AF_INET,	//Address family
			ip,			//IP address = 127.0.0.1
			&addr);		//Pointer of IP address

		if(result == 1){ //if no error,inet_pton function returns 1 and store network byte order to IN_ADDR structure.
			if(addr.S_un.S_addr != INADDR_NONE){ //Address is valid?
				ip_string = ip;
				hostname = ip;
				
				ip_bytes.resize(sizeof(ULONG));
				memcpy(&ip_bytes[0], &addr.S_un.S_addr, sizeof(ULONG));

				ipversion = IPVersion::IPv4;
				return;
			}
		}

		//Attempt to resolve hostname to ipv4 address.
		//hostname = www.google.com
		addrinfo hints = {}; //hints will filter the results we get back for getaddrinfo.
		hints.ai_family = AF_INET; //ipv4 adresses only
		addrinfo* hostinfo = nullptr;
		result = getaddrinfo(
			ip,				//IP address
			NULL,			//???
			&hints,			//Pointer of ADDRINFO structure
			&hostinfo);		//Result. Pointer of linked list of ADDRINFO structures
		if(result == 0){ //if no error, getaddrinfo function return 0.
			sockaddr_in *host_addr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);

			ip_string.resize(16);
			inet_ntop(AF_INET, &host_addr->sin_addr, &ip_string[0], 16);

			hostname = ip;
			ULONG ip_long = host_addr->sin_addr.S_un.S_addr;
			ip_bytes.resize(sizeof(ULONG));
			memcpy(&ip_bytes[0], &ip_long, sizeof(ULONG));

			ipversion = IPVersion::IPv4;

			freeaddrinfo(hostinfo);//free addrinfo structure
			return;
		}
	}

	IPEndpoint::IPEndpoint(sockaddr* addr){
		assert(addr->sa_family == AF_INET);
		sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(addr);
		ipversion = IPVersion::IPv4;
		port = ntohs(addrv4->sin_port);//network to host short.
		ip_bytes.resize(sizeof(ULONG));
		memcpy(&ip_bytes[0], &addrv4->sin_addr, sizeof(ULONG));
		ip_string.resize(16);
		inet_ntop(AF_INET, &addrv4->sin_addr, &ip_string[0], 16);
		hostname = ip_string;
		
		return;
	}

	IPVersion IPEndpoint::GetIPVersion(){
		return this->ipversion;
	}

	std::vector<uint8_t> IPEndpoint::GetIPBytes(){
		return this->ip_bytes;
	}

	std::string IPEndpoint::GetHostname(){
		return this->hostname;
	}

	std::string IPEndpoint::GetIPString(){
		return this->ip_string;
	}

	unsigned short IPEndpoint::GetPort(){
		return this->port;
	}
	
	//return SOCKADDR_IN structure which contains IP address and port, address family
	sockaddr_in IPEndpoint::GetSockaddrIPv4(){
		assert(ipversion == IPVersion::IPv4);
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr, &ip_bytes[0], sizeof(ULONG));
		addr.sin_port = htons(port);
		return addr;
	}

	void IPEndpoint::Print(){
		switch(ipversion){
		case IPVersion::IPv4:
			std::cout << "IP Version : IPv4" << std::endl;
			break;
		case IPVersion::IPv6:
			std::cout << "IP Version : IPv4" << std::endl;
			break;
		case IPVersion::Unknown:
			std::cout << "IP Version : Unknown" << std::endl;
			break;
		}
		std::cout << "Hostname : " << hostname << std::endl;
		std::cout << "IP : " << ip_string << std::endl;
		std::cout << "Port : " << port << std::endl;
		std::cout << "IP bytes..." << std::endl;
		for(auto& digit : ip_bytes)
			std::cout << (int)digit << std::endl;
	}
}