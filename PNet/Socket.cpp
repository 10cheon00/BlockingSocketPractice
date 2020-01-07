#include "Socket.h"
#include <assert.h>
namespace PNet {
	Socket::Socket(IPVersion ipversion, SocketHandle handle)
		:ipversion(ipversion), handle(handle) {
		assert(ipversion == IPVersion::IPv4);
	}
	PResult Socket::Create()
	{
		assert(ipversion == IPVersion::IPv4);
		if (handle != INVALID_SOCKET) {
			return PResult::P_NotYetImplemented;
		}

		handle = socket(
			AF_INET			/*if IPv6, AF_INET6*/,
			SOCK_STREAM,	//tcp
			IPPROTO_TCP);
		if (handle == INVALID_SOCKET) {
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}

	PResult Socket::Close() {
		if (handle == INVALID_SOCKET) {
			return PResult::P_NotYetImplemented;
		}

		int result = closesocket(handle);
		if (result != 0) {
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		handle = INVALID_SOCKET;
		return PResult::P_Success;
	}

	SocketHandle Socket::GetHandle()
	{
		return this->handle;
	}

	IPVersion Socket::GetIPVersion()
	{
		return this->ipversion;
	}
}
