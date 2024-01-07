#pragma once
#include "Proto.pb.h"

using PacketHandlerFunc = std::function<bool(std::shared_ptr<PacketSession>&, BYTE*, __int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];


enum : unsigned __int16
{
	PKT_S_ENTERGAME = 1000,
	PKT_S_LEAVEGAME = 1001,
	PKT_S_SPAWN = 1002,
	PKT_S_DESPAWN = 1003,
	PKT_C_MOVE = 1004,
	PKT_S_MOVE = 1005,
	PKT_C_SKILL = 1006,
	PKT_S_SKILL = 1007,
	PKT_S_DIE = 1008,
	PKT_S_CHANGEHP = 1009,
	PKT_S_PING = 1010,
	PKT_C_PONG = 1011,
};

// Custom Handlers
bool Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len);
bool Handle_S_ENTERGAME(std::shared_ptr<PacketSession>& session, Proto::S_ENTERGAME& pkt);
bool Handle_S_LEAVEGAME(std::shared_ptr<PacketSession>& session, Proto::S_LEAVEGAME& pkt);
bool Handle_S_SPAWN(std::shared_ptr<PacketSession>& session, Proto::S_SPAWN& pkt);
bool Handle_S_DESPAWN(std::shared_ptr<PacketSession>& session, Proto::S_DESPAWN& pkt);
bool Handle_S_MOVE(std::shared_ptr<PacketSession>& session, Proto::S_MOVE& pkt);
bool Handle_S_SKILL(std::shared_ptr<PacketSession>& session, Proto::S_SKILL& pkt);
bool Handle_S_DIE(std::shared_ptr<PacketSession>& session, Proto::S_DIE& pkt);
bool Handle_S_CHANGEHP(std::shared_ptr<PacketSession>& session, Proto::S_CHANGEHP& pkt);
bool Handle_S_PING(std::shared_ptr<PacketSession>& session, Proto::S_PING& pkt);

class ServerProtocolManager
{
public:
	static void Init()
	{
		for (__int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_ENTERGAME] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::S_ENTERGAME>(Handle_S_ENTERGAME, session, buffer, len); };
		GPacketHandler[PKT_S_LEAVEGAME] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::S_LEAVEGAME>(Handle_S_LEAVEGAME, session, buffer, len); };
		GPacketHandler[PKT_S_SPAWN] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::S_SPAWN>(Handle_S_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_S_DESPAWN] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::S_DESPAWN>(Handle_S_DESPAWN, session, buffer, len); };
		GPacketHandler[PKT_S_MOVE] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::S_MOVE>(Handle_S_MOVE, session, buffer, len); };
		GPacketHandler[PKT_S_SKILL] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::S_SKILL>(Handle_S_SKILL, session, buffer, len); };
		GPacketHandler[PKT_S_DIE] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::S_DIE>(Handle_S_DIE, session, buffer, len); };
		GPacketHandler[PKT_S_CHANGEHP] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::S_CHANGEHP>(Handle_S_CHANGEHP, session, buffer, len); };
		GPacketHandler[PKT_S_PING] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::S_PING>(Handle_S_PING, session, buffer, len); };
	}

	static bool HandlePacket(std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::C_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_C_MOVE); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::C_SKILL& pkt) { return MakeSendBuffer(pkt, PKT_C_SKILL); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::C_PONG& pkt) { return MakeSendBuffer(pkt, PKT_C_PONG); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static std::shared_ptr<SendBuffer> MakeSendBuffer(T& pkt, unsigned __int16 pktId)
	{
		const unsigned __int16 dataSize = static_cast<unsigned __int16>(pkt.ByteSizeLong());
		const unsigned __int16 packetSize = dataSize + sizeof(PacketHeader);

		std::shared_ptr<SendBuffer> sendBuffer = std::make_shared<SendBuffer>(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->SetWriteSize(packetSize);

		return sendBuffer;
	}
};

