#include "stdafx.h"
#include "ClientSessionManager.h"
#include "ClientSession.h"

ClientSessionManager GSessionManager;

void ClientSessionManager::Add(std::shared_ptr<ClientSession> session)
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);
	_sessions.insert(session);
	std::cout << "Connected " << _sessions.size() << " Player" << std::endl;
}

void ClientSessionManager::Remove(std::shared_ptr<ClientSession> session)
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);
	_sessions.erase(session);
	std::cout << "Connected " << _sessions.size() << "Player" << std::endl;
}

void ClientSessionManager::Broadcast(std::shared_ptr<SendBuffer> sendBuffer)
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);
	for (std::shared_ptr<ClientSession> session : _sessions)
	{
		session->Send(sendBuffer);
	}
}