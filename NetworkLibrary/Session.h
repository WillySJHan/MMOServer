#pragma once
#include <shared_mutex>
#include <mutex>
#include <queue>

#include "IocpBase.h"
#include "IocpEvent.h"
#include "SocketAddress.h"
#include "RecvBuffer.h"



class Service;

class Session : public IocpObject
{
	friend class IocpBase;
	friend class Service;
	friend class Listener;

	enum
	{
		BUFFER_SIZE = 0x10000, // 64KB
	};


public:
	Session();
	virtual ~Session();

public:

	
	void				Send(std::shared_ptr<class SendBuffer> sendBuffer);
	bool				Connect();
	void				Disconnect(const WCHAR* cause);

	std::shared_ptr<Service>	GetService() { return _service.lock(); }
	void				SetService(std::shared_ptr<Service> service) { _service = service; }

public:
	
	void						SetSocketAddr(SocketAddress address) { _socketAddr = address; }
	SocketAddress					GetAddress() { return _socketAddr; }
	SOCKET						GetSocket() { return _socket; }
	bool						IsConnected() { return _connected; }
	std::shared_ptr<Session>	GetSessionPtr() { return std::static_pointer_cast<Session>(shared_from_this()); }

private:
	
	virtual HANDLE				GetHandle() override;
	virtual void				Dispatch(class IocpEvent* iocpEvent, __int32 numOfBytes = 0) override;

private:
	
	bool				RegisterConnect();
	bool				RegisterDisconnect();
	void				RegisterRecv();
	void				RegisterSend();

	void				ProcessConnect();
	void				ProcessDisconnect();
	void				ProcessRecv(__int32 numOfBytes);
	void				ProcessSend(__int32 numOfBytes);

	void				HandleError(__int32 errorCode);

protected:

	virtual void		OnConnected() { }
	virtual __int32		OnRecv(unsigned char* buffer, __int32 len) { return len; }
	virtual void		OnSend(__int32 len) { }
	virtual void		OnDisconnected() { }

public:



private:
	std::weak_ptr<Service> _service;
	SOCKET			_socket = INVALID_SOCKET;
	SocketAddress		_socketAddr = {};
	std::atomic<bool>	_connected = false;



private:
	std::recursive_mutex _rMutex;
	std::shared_mutex _sMutex;


	RecvBuffer _recvBuffer;


	std::queue<std::shared_ptr<SendBuffer>> _sendQueue;
	std::atomic<bool> _sendRegistered = false;

private:

	ConnectEvent		_connectEvent;
	DisconnectEvent		_disconnectEvent;
	RecvEvent			_recvEvent;
	SendEvent			_sendEvent;
};



struct PacketHeader
{
	unsigned __int16 size;
	unsigned __int16 id;
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	std::shared_ptr<PacketSession>	GetPacketSessionSptr() { return std::static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual __int32		OnRecv(unsigned char* buffer, __int32 len) sealed;
	virtual void		OnRecvPacket(unsigned char* buffer, __int32 len) abstract;
};

