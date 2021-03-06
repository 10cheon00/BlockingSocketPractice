//server code

#include <Pnet\IncludeMe.h>
#include <iostream>

using namespace PNet;

bool ProcessPacket(Packet& packet){
	switch(packet.GetPacketType()){
	case PacketType::PT_ChatMessage:
	{
		std::string chatmessage;
		packet >> chatmessage;
		std::cout << "Chat Message : " << chatmessage << std::endl;
	}
	break;
	case PacketType::PT_IntegerArray:
	{
		uint32_t arraySize = 0;
		packet >> arraySize;
		std::cout << "ArraySize : " << arraySize << std::endl;
		for(uint32_t i = 0; i < arraySize; i++){
			uint32_t element = 0;
			packet >> element;
			std::cout << "Element[" << i << "] = " << element << std::endl;
		}
		break;
	}
	default: return false;
	}
	return true;
}

int main(){
	if(Network::Initialize()){
		std::cout << "Winsock api successfully initialized." << std::endl;

		/*
		//server to listen for connection on port 8574
		//socket  - bind to 8574

		IPEndpoint test("www.naver.com",8574);
		if(test.GetIPVersion() == IPVersion::IPv4){
			std::cout << "Hostname : " << test.GetHostname() << std::endl;
			std::cout << "IP : " << test.GetIPString() << std::endl;
			std::cout << "Port : " << test.GetPort() << std::endl;
			std::cout << "IPBytes..." << std::endl;
			for(auto& digit : test.GetIPBytes()){
				std::cout << (int)digit << std::endl;
			}
		}
		else{
			std::cerr << "This is not an ipv4 address." << std::endl;
		}
		*/

		Socket socket(IPVersion::IPv6);
		if(socket.Create() == PResult::P_Success){
			std::cout << "Socket successfully created." << std::endl;
			if(socket.Listen(IPEndpoint("::", 8574)) == PResult::P_Success){
				std::cout << "Socket successfully listening on port 8574." << std::endl;
				Socket newConnection;
				if(socket.Accept(newConnection) == PResult::P_Success){
					std::cout << "New connection accepted." << std::endl;
					/*
					std::string buffer = "";
					while(true){
						uint32_t bufferSize = 0;
						//1. Receive bufferSize to be received next.
						int result = newConnection.RecvAll(&bufferSize, sizeof(uint32_t));
						if(result != PResult::P_Success)
							break;
						bufferSize = ntohl(bufferSize);
						//If client want to send data more than maximum value, break connection.
						if(bufferSize > PNet::g_MaxPacketSize)
							break;
						buffer.resize(bufferSize);
						//2. Receive data.
						result = newConnection.RecvAll(&buffer[0], bufferSize);
						if(result != PResult::P_Success)
							break;
						std::cout << '[' << bufferSize << "] : " << buffer << std::endl;
					}
					*/

					Packet pkt;
					while(true){
						PResult result = newConnection.Recv(pkt);
						if(result != PResult::P_Success)
							break;
						if(!ProcessPacket(pkt))
							break;
					}
					newConnection.Close();
				}
			}
			else{
				std::cout << "Failed to accepted new Connection." << std::endl;
			}
		}
		else
			std::cout << "Failed to listening on port 8574." << std::endl;
		socket.Close();
	}
	else{
		std::cerr << "Socket failed to create." << std::endl;
	}


	Network::Shutdown();
	system("Pause");
	return 0;
}