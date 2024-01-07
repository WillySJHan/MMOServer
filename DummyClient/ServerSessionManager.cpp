#include "stdafx.h"
#include "ServerSessionManager.h"
#include "ServerSession.h"


ServerSessionManager GSessionManager;



void ServerSessionManager::Add(std::shared_ptr<ServerSession> session)
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);
	_sessions.insert(session);
	session->_dummyId = _dummyId;
	_dummyId++;
	std::cout << "Connected " << _sessions.size() << " Dummy Player" <<std::endl;
}

void ServerSessionManager::Remove(std::shared_ptr<ServerSession> session)
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);
	_sessions.erase(session);
	std::cout << "Connected " << _sessions.size() << " Dummy Player" << std::endl;
}

void ServerSessionManager::Broadcast(std::shared_ptr<SendBuffer> sendBuffer)
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);
	for (std::shared_ptr<ServerSession> session : _sessions)
	{
		session->Send(sendBuffer);
	}
}
