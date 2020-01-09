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
			return PResult::P_NotYetImplemented;
		}

		handle = socket(	//create socket function
			AF_INET,		//Address family 
			SOCK_STREAM,	//type
			IPPROTO_TCP);	//protocol
		if(handle == INVALID_SOCKET){
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		if(SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Success){
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}

	PResult Socket::Close(){
		if(handle == INVALID_SOCKET){
			return PResult::P_NotYetImplemented;
		}

		int result = closesocket(handle);
		if(result != 0){
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		handle = INVALID_SOCKET;
		return PResult::P_Success;
	}

	PResult Socket::Listen(IPEndpoint endpoint, int backlog){
		if(Bind(endpoint) != PResult::P_Success) 
			return PResult::P_NotYetImplemented;

		int result = listen(
			handle, 
			backlog);	//maximum length of the queue of pending connections.
		if(result != 0){
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
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
			return PResult::P_NotYetImplemented;
			int error = WSAGetLastError();
		}

		IPEndpoint newConnectionEndpoint((sockaddr*)&addr);
		std::cout << "New connection accepted" << std::endl;
		newConnectionEndpoint.Print();
		outSocket = Socket(IPVersion::IPv4, acceptedConnectionHandle);
		return PResult::P_Success;
	}

	PResult Socket::Connect(IPEndpoint endpoint){
		sockaddr_in addr = endpoint.GetSockaddrIPv4();
		int result = connect(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if(result != 0){
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		return PResult::P_Success;
	}

	
	PResult Socket::Bind(IPEndpoint endpoint){
		sockaddr_in addr = endpoint.GetSockaddrIPv4();
		int result = bind(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if(result != 0){
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
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
			return PResult::P_NotYetImplemented;
		}
		if(result != 0){
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}
}
