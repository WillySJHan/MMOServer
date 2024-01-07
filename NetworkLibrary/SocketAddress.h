#pragma once


class SocketAddress
{
public:
	SocketAddress() = default;
	SocketAddress(SOCKADDR_IN SocketAddress);
	SocketAddress(std::wstring ip, unsigned __int16 port);

	SOCKADDR_IN& GetSocketAddress() { return _socketAddress; }
	std::wstring			GetIpAddress();
	unsigned __int16			GetPort() { return ::ntohs(_socketAddress.sin_port); }

public:
	static IN_ADDR	IpToAddress(const WCHAR* ip);

private:
	SOCKADDR_IN		_socketAddress = {};
};

