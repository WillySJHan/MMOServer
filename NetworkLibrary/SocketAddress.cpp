#include "pch.h"
#include "SocketAddress.h"



SocketAddress::SocketAddress(SOCKADDR_IN sockAddr) : _socketAddress(sockAddr)
{
}

SocketAddress::SocketAddress(std::wstring ip, unsigned __int16 port)
{
	::memset(&_socketAddress, 0, sizeof(_socketAddress));
	_socketAddress.sin_family = AF_INET;
	
	if (ip == L"127.0.0.2")
		_socketAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	else
		_socketAddress.sin_addr = IpToAddress(ip.c_str());
	//inet_pton(AF_INET, "58.120.83.156", &_socketAddress.sin_addr);
	_socketAddress.sin_port = ::htons(port);
}

std::wstring SocketAddress::GetIpAddress()
{
	WCHAR buffer[100];
	::InetNtopW(AF_INET, &_socketAddress.sin_addr, buffer, sizeof(buffer) / sizeof(WCHAR));
	return std::wstring(buffer);
}

IN_ADDR SocketAddress::IpToAddress(const WCHAR* ip)
{
	IN_ADDR address;
	::InetPtonW(AF_INET, ip, &address);
	return address;
}
