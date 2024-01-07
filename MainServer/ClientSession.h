#pragma once
#include "Session.h"

class ClientSession : public PacketSession
{
public:
	~ClientSession()
	{
		std::cout << "~ClientSession" << std::endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, __int32 len) override;
	virtual void OnSend(__int32 len) override;

	void Ping();
	void HandlePong();

public:
	//std::weak_ptr<class Player> _myPlayer;
	std::weak_ptr<class Player> _myPlayer;
	unsigned __int64 _pingPongTick = 0;
	//bool pingCancel = false;
	std::recursive_mutex _rMutex;
	//std::weak_ptr<class Room> _room;
	//std::shared_ptr<class Task> _task;
};