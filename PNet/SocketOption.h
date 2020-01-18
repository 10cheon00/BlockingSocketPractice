#pragma once
namespace PNet{
	enum SocketOption{
		TCP_NoDelay,	//TRUE = disable Nagle's algoritm.
		IPv6_only,		//TRUE = Only ipv6 can connect. FALSE = use both.
	};
}