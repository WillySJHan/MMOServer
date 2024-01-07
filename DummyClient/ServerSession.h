#pragma once
#include "ServerProtocolManager.h"

class ServerSession : public PacketSession
{
public:
	~ServerSession();

	virtual void OnConnected() override;

	virtual void OnRecvPacket(BYTE* buffer, __int32 len) override;

	virtual void OnSend(__int32 len) override;

	virtual void OnDisconnected() override;

public:
	__int32 _dummyId = 0;
};
