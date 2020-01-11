//client code

#include <Pnet\IncludeMe.h>
#include <iostream>

using namespace PNet;

int main(){
	if(Network::Initialize()){
		std::cout << "Winsock api successfully initialized." << std::endl;
		Socket socket;
		if(socket.Create() == PResult::P_Success){
			std::cout << "Socket successfully created." << std::endl;
			if(socket.Connect(IPEndpoint("127.0.0.1", 8574)) == PResult::P_Success){
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
				Packet pkt;
				pkt << std::string("this is first string!");
				pkt << std::string("this is second string!");
				while(true){
					PResult result = socket.Send(pkt);
					if(result != PResult::P_Success)
						break;
					std::cout << "Attempting to send chunk of data.." << std::endl;
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