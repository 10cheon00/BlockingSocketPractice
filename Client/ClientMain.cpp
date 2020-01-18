//client code

#include <Pnet\IncludeMe.h>
#include <iostream>

using namespace PNet;

int main(){
	if(Network::Initialize()){
		std::cout << "Winsock api successfully initialized." << std::endl;
		Socket socket(IPVersion::IPv6);
		if(socket.Create() == PResult::P_Success){
			std::cout << "Socket successfully created." << std::endl;
			if(socket.Connect(IPEndpoint("::1", 8574)) == PResult::P_Success){
				std::cout << "Successfully connected to server!" << std::endl;
				/*
				std::string buffer = "";

				buffer.resize(PNet::g_MaxPacketSize + 1);
				memset(&buffer[0], 'A', PNet::g_MaxPacketSize + 1);
				while(true){
					//1. Send bufferSize first.
					uint32_t bufferSize = buffer.size();
					bufferSize = htonl(bufferSize);
					int result = socket.SendAll(&bufferSize, sizeof(uint32_t));
					if(result != PResult::P_Success)
						break;

					//2. Secondly, send data. Size must equal to previous sending bufferSize.
					result = socket.SendAll(buffer.data(), buffer.size());
					if(result != PResult::P_Success)
						break;
					std::cout << "Attempting to send chunk of data..." << std::endl;
					Sleep(500);
				}
				*/

				Packet stringPacket(PacketType::PT_ChatMessage);
				stringPacket << std::string("This is my string packet~~");
				
				Packet integerPacket(PacketType::PT_IntegerArray);
				uint32_t arraySize = 9;
				uint32_t integerArray[9] = { 2,5,134,414,1,3,45,489,41 };
				integerPacket << arraySize;
				for(auto iter : integerArray)
					integerPacket << iter;

				while(true){
					PResult result;
					if(rand() % 2 == 0){
						result = socket.Send(stringPacket);
					}
					else{
						result = socket.Send(integerPacket);
					}

					if(result != PResult::P_Success)
						break;

					std::cout << "Attempting to send data.." << std::endl;
					Sleep(500);
				}
			}
			else{
				std::cout << "Failed to connected to server." << std::endl;
			}
			socket.Close();
		}
		else{
			std::cerr << "Socket failed to create." << std::endl;
		}
	}

	Network::Shutdown();
	system("pause");
	return 0;
};