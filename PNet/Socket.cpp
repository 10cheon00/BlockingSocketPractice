#include "Socket.h"
#include <assert.h>
#include <iostream>

namespace PNet {
	Socket::Socket(IPVersion ipversion, SocketHandle handle)
		:ipversion(ipversion), handle(handle){
		assert(ipversion == IPVersion::IPv4);
	}

	PResult Socket::Create(){
		assert(ipversion == IPVersion::IPv4);
		if(handle != INVALID_SOCKET){
			return PResult::P_GenericError;
		}

		handle = socket(	//create socket function
			AF_INET,		//Address family 
			SOCK_STREAM,	//type
			IPPROTO_TCP);	//protocol
		if(handle == INVALID_SOCKET){
			int err = WSAGetLastError();
			return PResult::P_GenericError;
		}

		if(SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Success){
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::Close(){
		if(handle == INVALID_SOCKET){
			return PResult::P_GenericError;
		}

		int result = closesocket(handle);
		if(result != 0){
			int err = WSAGetLastError();
			return PResult::P_GenericError;
		}

		handle = INVALID_SOCKET;
		return PResult::P_Success;
	}

	PResult Socket::Listen(IPEndpoint endpoint, int backlog){
		if(Bind(endpoint) != PResult::P_Success) //if Bind fails
			return PResult::P_GenericError;

		int result = listen(
			handle,		//SOCKET
			backlog);	//maximum length of the queue of pending connections.
		if(result != 0){
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::Accept(Socket& outSocket){
		/***************** SOCKADDR_IN *****************

		struct SOCKADDR_IN{
			short			sin_family;		//Address family
			unsigned short	sin_port;		//IP port
			struct IN_ADDR	sin_addr;		//IP address
			char			sin_zero[8];	//make this structure size same as SOCKADDR
		}

		***********************************************/
		sockaddr_in addr = {};
		int len = sizeof(sockaddr_in);
		
		//connect client to new socket.
		SocketHandle acceptedConnectionHandle = accept(
			handle,				//SOCKET
			(sockaddr*)(&addr), //sockaddr pointer to receive address of client
			&len);				//sizeof sockaddr
		if(acceptedConnectionHandle == INVALID_SOCKET){
			return PResult::P_GenericError;
			int error = WSAGetLastError();
		}

		IPEndpoint newConnectionEndpoint((sockaddr*)&addr);
		std::cout << "New connection accepted" << std::endl;
		newConnectionEndpoint.Print();
		outSocket = Socket(IPVersion::IPv4, acceptedConnectionHandle);//copy socket
		return PResult::P_Success;
	}

	PResult Socket::Connect(IPEndpoint endpoint){
		sockaddr_in addr = endpoint.GetSockaddrIPv4();
		int result = connect(
			handle,					//SOCKET
			(sockaddr*)(&addr),		//Pointer of sockaddr structure(must have connected)
			sizeof(sockaddr_in));	//sizeof sockaddr structure
		if(result != 0){
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}//If error occured
		return PResult::P_Success;
	}

	PResult Socket::Send(const void* data, int numberOfBytes, int& bytesSent){
		bytesSent = send(
			handle,				//SOCKET
			(const char*)data,	//Pointer to send data
			numberOfBytes,		//length of data
			NULL);				//Pointer to store length of data what actually sent

		if(bytesSent == SOCKET_ERROR){
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::Recv(void* destination, int numberOfBytes, int& bytesReceived){
		bytesReceived = recv(
			handle,				//SOCKET
			(char*)destination,	//Pointer to store received data
			numberOfBytes,		//length of data to receive
			NULL);				//Pointer to store length of data what actually received

		if(bytesReceived == 0)//If connection was gracefully closed.
			return PResult::P_GenericError;
		if(bytesReceived == SOCKET_ERROR){
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::Send(Packet& packet){
		//Send packet size.
		uint32_t encodePakcetSize = htonl(packet.buffer.size());
		PResult result = SendAll(&encodePakcetSize, sizeof(uint32_t));
		if(result != PResult::P_Success)
			return PResult::P_GenericError;

		//Send data.
		result = SendAll(packet.buffer.data(), packet.buffer.size());
		if(result != PResult::P_Success)
			return PResult::P_GenericError;

		return PResult::P_Success;
	}

	//packet structure = size + data
	PResult Socket::Recv(Packet& packet){
		packet.Clear();

		//Receive data size. 
		uint32_t encodedSize = 0;
		PResult result = RecvAll(&encodedSize, sizeof(uint32_t));
		if(result != PResult::P_Success)
			return PResult::P_GenericError;

		//Get data.
		uint32_t bufferSize = ntohl(encodedSize);
		packet.buffer.resize(bufferSize);
		result = RecvAll(&packet.buffer[0], bufferSize);
		if(result != PResult::P_Success)
			return PResult::P_GenericError;

		return PResult::P_Success;
	}

	PResult Socket::SendAll(const void* data, int numberOfBytes){
		int totalBytesSent = 0;
		while(totalBytesSent < numberOfBytes){
			int bytesRemaining = numberOfBytes - totalBytesSent;
			int bytesSent = 0;
			char* bufferOffset = (char*)data + totalBytesSent;
			PResult result = Send(bufferOffset, bytesRemaining, bytesSent);
			if(result != PResult::P_Success){
				return PResult::P_GenericError;
			}
			totalBytesSent += bytesSent;
		}
		return PResult::P_Success;
	}

	PResult Socket::RecvAll(void* destination, int numberOfBytes){
		int totalBytesReceived = 0;
		while(totalBytesReceived < numberOfBytes){
			int bytesRemaining = numberOfBytes - totalBytesReceived;
			int bytesReceived = 0;
			char* bufferOffset = (char*)destination + totalBytesReceived;
			PResult result = Recv(bufferOffset, bytesRemaining, bytesReceived);
			if(result != PResult::P_Success){
				return PResult::P_GenericError;
			}
			totalBytesReceived += bytesReceived;
		}
		return PResult::P_Success;
	}

	PResult Socket::Bind(IPEndpoint endpoint){
		sockaddr_in addr = endpoint.GetSockaddrIPv4();

		//call bind function before call listen function.
		int result = bind(
			handle,					//SOCKET
			(sockaddr*)(&addr),		//Host address Structure
			sizeof(sockaddr_in));	//sizeof sockaddr structure
		if(result != 0){
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	SocketHandle Socket::GetHandle(){
		return this->handle;
	}

	IPVersion Socket::GetIPVersion(){
		return this->ipversion;
	}

	PResult Socket::SetSocketOption(SocketOption option, BOOL value){
		int result = 0;
		switch(option){
		case SocketOption::TCP_NoDelay:
			result = setsockopt(
				handle,					//Socket
				IPPROTO_TCP,			//
				TCP_NODELAY,			//Socket Option
				(const char*)&value,	//Option buffer pointer
				sizeof(value));			//buffer size
			break;
		default:
			return PResult::P_GenericError;
		}
		if(result != 0){
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}
}
