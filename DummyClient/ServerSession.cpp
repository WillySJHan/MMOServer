#include "stdafx.h"
#include "ServerSession.h"
#include "ServerSessionManager.h"

ServerSession::~ServerSession()
{
	std::cout << "~ServerSession" << std::endl;
}

void ServerSession::OnConnected()
{
	std::cout << "onConnected" << std::endl;
	GSessionManager.Add(std::static_pointer_cast<ServerSession>(shared_from_this()));


}

void ServerSession::OnRecvPacket(BYTE* buffer, int len)
{
	std::shared_ptr<PacketSession> session = GetPacketSessionSptr();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	ServerProtocolManager::HandlePacket(session, buffer, len);
}

void ServerSession::OnSend(int len)
{

}

void ServerSession::OnDisconnected()
{
	std::cout << "Disconnected" << std::endl;
}
