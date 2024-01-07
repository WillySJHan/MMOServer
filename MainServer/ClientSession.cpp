#include "stdafx.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "ClientProtocolManager.h"
#include "DataManager.h"
#include "Room.h"
#include "Player.h"
#include "ObjectManager.h"
#include "RoomManager.h"
#include "VisualField.h"

void ClientSession::OnConnected()
{
	GSessionManager.Add(std::static_pointer_cast<ClientSession>(shared_from_this()));
	
	std::cout << "onConnected" << std::endl;
	_myPlayer = ObjectManager::Instance()->Add<Player>();
	
	if (auto myPlayer = _myPlayer.lock())
	{
		myPlayer->_info->set_name("Player_" + std::to_string(myPlayer->_info->objectid()));
		myPlayer->_info->mutable_posinfo()->set_state(Proto::CreatureState::IDLE);
		myPlayer->_info->mutable_posinfo()->set_movedir(Proto::MoveDir::DOWN);
		myPlayer->_info->mutable_posinfo()->set_posx(0);
		myPlayer->_info->mutable_posinfo()->set_posy(0);

		/////////////////////////////////////////////////////////////////////
		Proto::StatInfo stat = DataManager::StatMap[1];
		myPlayer->_info->mutable_statinfo()->MergeFrom(stat);

		myPlayer->_ownerSession = std::static_pointer_cast<ClientSession>(shared_from_this());
		myPlayer->_visualField = std::make_shared<VisualField>(myPlayer);
	

	std::shared_ptr<BaseObject> baseObject = std::static_pointer_cast<BaseObject>(myPlayer);

	RoomManager::Instance()->Find(1)->DoAsync((&Room::Enter), baseObject, true);
	RoomManager::Instance()->Find(1)->DoAsync( &Room::PingCheck, myPlayer);
	}
}

void ClientSession::OnDisconnected()
{
	std::cout << "onDisConnected" << std::endl;
	if (auto myPlayer = _myPlayer.lock())
	{

		if (myPlayer->_visualField->_task != nullptr)
		{
			myPlayer->_visualField->_task->_cancel = true;
			myPlayer->_visualField->_task = nullptr;
		}


		if (myPlayer == nullptr)
			return;
		std::shared_ptr<BaseObject> baseObject = std::static_pointer_cast<BaseObject>(myPlayer);


		RoomManager::Instance()->Find(1)->DoAsync((&Room::Leave), baseObject->_info->objectid());



		GSessionManager.Remove(std::static_pointer_cast<ClientSession>(shared_from_this()));
		RoomManager::Instance()->Find(1)->DoAsync(&Room::Remove, baseObject->_info->objectid());

		//RoomManager::Instance()->Find(1)->Remove(baseObject->_info->objectid());
		ObjectManager::Instance()->Remove(baseObject->_info->objectid());
		//RoomManager::Instance()->Find(1)->DoTimer(3000, &Room::ClearMemory, std::static_pointer_cast<ClientSession>(shared_from_this()));
		myPlayer->_visualField->_previousObjects.clear();
	}
}


void ClientSession::OnRecvPacket(BYTE* buffer, __int32 len)
{
	std::shared_ptr<PacketSession> session = GetPacketSessionSptr();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ClientProtocolManager::HandlePacket(session, buffer, len);
}

void ClientSession::OnSend(__int32 len)
{

}

void ClientSession::Ping()
{

	if (_pingPongTick > 0)
	{
		__int64 delta = (::GetTickCount() - _pingPongTick);
		if (delta > 30 * 1000)
		{
			//pingCancel = true;
			Disconnect(L"Disconnected PingPong");
			return;
		}
	}

	Proto::S_PING pingPacket;
	auto sendBuffer = ClientProtocolManager::MakeSendBuffer(pingPacket);

	Send(sendBuffer);


	if (auto myPlayer = _myPlayer.lock())
	{
		RoomManager::Instance()->Find(1)->DoTimer(5000, &Room::PingCheck, myPlayer);
	}
	
}

void ClientSession::HandlePong()
{
	_pingPongTick = ::GetTickCount();
}
