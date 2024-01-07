#include "stdafx.h"
#include "ServerProtocolManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}


bool Handle_S_ENTERGAME(std::shared_ptr<PacketSession>& session, Proto::S_ENTERGAME& pkt)
{
	Proto::S_ENTERGAME enterGamePacket;

	return true;
}

bool Handle_S_LEAVEGAME(std::shared_ptr<PacketSession>& session, Proto::S_LEAVEGAME& pkt)
{
	Proto::S_LEAVEGAME leaveGamePacket;
	return true;
}

bool Handle_S_SPAWN(std::shared_ptr<PacketSession>& session, Proto::S_SPAWN& pkt)
{
	return true;
}

bool Handle_S_DESPAWN(std::shared_ptr<PacketSession>& session, Proto::S_DESPAWN& pkt)
{
	return true;
}

bool Handle_S_MOVE(std::shared_ptr<PacketSession>& session, Proto::S_MOVE& pkt)
{
	return true;
}

bool Handle_S_SKILL(std::shared_ptr<PacketSession>& session, Proto::S_SKILL& pkt)
{
	return true;
}

bool Handle_S_DIE(std::shared_ptr<PacketSession>& session, Proto::S_DIE& pkt)
{
	return true;
}

bool Handle_S_CHANGEHP(std::shared_ptr<PacketSession>& session, Proto::S_CHANGEHP& pkt)
{
	return true;
}

bool Handle_S_PING(std::shared_ptr<PacketSession>& session, Proto::S_PING& pkt)
{
	return true;
}





