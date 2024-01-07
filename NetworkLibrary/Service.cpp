#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"



Service::Service(ServiceType type, SocketAddress address, std::shared_ptr<IocpBase> base, SessionFactory factory, __int32 maxSessionCount)
	: _type(type), _socketAddr(address), _iocpBase(base), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{
}

void Service::CloseService()
{

}

void Service::Broadcast(std::shared_ptr<SendBuffer> sendBuffer)
{
	std::lock_guard<std::shared_mutex> lock(_sMutex);
	for (const auto& session : _sessions)
	{
		session->Send(sendBuffer);
	}
}

std::shared_ptr<Session> Service::CreateSession()
{
	std::shared_ptr<Session> session = _sessionFactory();
	session->SetService(shared_from_this());

	if (_iocpBase->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(std::shared_ptr<Session> session)
{
	std::lock_guard<std::shared_mutex> lock(_sMutex);
	_sessionCount++;
	_sessions.insert(session);
}

void Service::ReleaseSession(std::shared_ptr<Session> session)
{
	std::lock_guard<std::shared_mutex> lock(_sMutex);
	
	_sessions.erase(session);
	_sessionCount--;
}



ClientService::ClientService(SocketAddress targetAddress, std::shared_ptr<IocpBase> base, SessionFactory factory, __int32 maxSessionCount)
	: Service(ServiceType::Client, targetAddress, base, factory, maxSessionCount)
{
}

bool ClientService::Start()
{
	if (CanStart() == false)
		return false;

	const __int32 sessionCount = GetMaxSessionCount();
	for (__int32 i = 0; i < sessionCount; i++)
	{
		std::shared_ptr<Session> session = CreateSession();
		if (session->Connect() == false)
			return false;
	}

	return true;
}

ServerService::ServerService(SocketAddress serverAddress, std::shared_ptr<IocpBase> base, SessionFactory factory, __int32 maxSessionCount)
	: Service(ServiceType::Server, serverAddress, base, factory, maxSessionCount)
{
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = std::make_shared<Listener>();
	if (_listener == nullptr)
		return false;

	std::shared_ptr<ServerService> service = std::static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{


	Service::CloseService();
}
