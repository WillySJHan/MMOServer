#pragma once
#include <set>
#include <shared_mutex>
#include <mutex>

class ClientSession;


class ClientSessionManager
{
public:
	void Add(std::shared_ptr<ClientSession> session);
	void Remove(std::shared_ptr<ClientSession> session);
	void Broadcast(std::shared_ptr<SendBuffer> sendBuffer);

private:
	std::recursive_mutex _rMutex;
	std::shared_mutex _sMutex;
	std::set<std::shared_ptr<ClientSession>> _sessions;
};

extern ClientSessionManager GSessionManager;
