#pragma once
#include <vector>
class Session;

enum class EventType : unsigned __int8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv,
	Recv,
	Send
};



class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void			Init();

public:
	EventType eventType;
	std::shared_ptr<class IocpObject> owner;
};



class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};


class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) { }
};


class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }

public:
	std::shared_ptr<class Session> session = nullptr;
};



class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};



class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }

	std::vector<std::shared_ptr<class SendBuffer>> sendBuffers;
};