#include "stdafx.h"
#include "ClientProtocolManager.h"
#include "Player.h"
#include "Room.h"
#include "ClientSession.h"
#include "Room.h"
#include "RoomManager.h"
#include "Player.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_MOVE(std::shared_ptr<PacketSession>& session, Proto::C_MOVE& pkt)
{
	
	std::shared_ptr<ClientSession> clientSession = std::static_pointer_cast<ClientSession>(session);


	if (auto myPlayer = clientSession->_myPlayer.lock())
	{
		std::shared_ptr<Room> room = myPlayer->_room;
		if (room == nullptr)
			return false;

		room->DoAsync((&Room::ManageMove), myPlayer, pkt);
	}

	


	return true;
}

bool Handle_C_SKILL(std::shared_ptr<PacketSession>& session, Proto::C_SKILL& pkt)
{
	std::shared_ptr<ClientSession> clientSession = std::static_pointer_cast<ClientSession>(session);


	if (auto myPlayer = clientSession->_myPlayer.lock())
	{
		std::shared_ptr<Room> room = myPlayer->_room;
		if (room == nullptr)
			return false;

		room->DoAsync((&Room::ManageSkill), myPlayer, pkt);

	}



	return true;
}

bool Handle_C_PONG(std::shared_ptr<PacketSession>& session, Proto::C_PONG& pkt)
{
	std::shared_ptr<ClientSession> clientSession = std::static_pointer_cast<ClientSession>(session);

	clientSession->HandlePong();


	return true;
}





