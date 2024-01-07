#pragma once
#include <set>
#include <shared_mutex>
#include <mutex>

class ServerSessionManager
{
public:
	void Add(std::shared_ptr<class ServerSession> session);
	void Remove(std::shared_ptr<class ServerSession> session);
	void Broadcast(std::shared_ptr<SendBuffer> sendBuffer);

private:
	std::recursive_mutex _rMutex;
	std::shared_mutex _sMutex;
	std::set<std::shared_ptr<class ServerSession>> _sessions;
	__int32 _dummyId = 1;
};

extern ServerSessionManager GSessionManager;




