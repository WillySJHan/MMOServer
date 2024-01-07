#pragma once
#include "SocketAddress.h"
#include "IocpBase.h"
#include "Listener.h"
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <set>

enum class ServiceType : unsigned __int8
{
	Server,
	Client
};



using SessionFactory = std::function<std::shared_ptr<class Session>(void)>;

class Service : public std::enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, SocketAddress address, std::shared_ptr<IocpBase> base, SessionFactory factory, __int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool		Start() abstract;
	bool				CanStart() { return _sessionFactory != nullptr; }

	virtual void		CloseService();
	void				SetSessionFactory(SessionFactory func) { _sessionFactory = func; }



	void				Broadcast(std::shared_ptr<SendBuffer> sendBuffer);
	std::shared_ptr<class Session>			CreateSession();
	void				AddSession(std::shared_ptr<class Session> session);
	void				ReleaseSession(std::shared_ptr<class Session> session);
	__int32				GetCurrentSessionCount() { return _sessionCount; }
	__int32				GetMaxSessionCount() { return _maxSessionCount; }

public:
	ServiceType			GetServiceType() { return _type; }
	SocketAddress			GetSocketAddr() { return _socketAddr; }
	std::shared_ptr<IocpBase>& GetIocpBase() { return _iocpBase; }

protected:
	std::recursive_mutex _rMutex;
	std::shared_mutex _sMutex;
	ServiceType			_type;
	SocketAddress			_socketAddr = {};
	std::shared_ptr<IocpBase>			_iocpBase;

	std::set<std::shared_ptr<class Session>>		_sessions;
	__int32				_sessionCount = 0;
	__int32				_maxSessionCount = 0;
	SessionFactory		_sessionFactory;
};



class ClientService : public Service
{
public:
	ClientService(SocketAddress targetAddress, std::shared_ptr<IocpBase> base, SessionFactory factory, __int32 maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool	Start() override;
};




class ServerService : public Service
{
public:
	ServerService(SocketAddress serverAddress, std::shared_ptr<IocpBase> base, SessionFactory factory, __int32 maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool	Start() override;
	virtual void	CloseService() override;

private:
	std::shared_ptr<Listener>		_listener = nullptr;
};