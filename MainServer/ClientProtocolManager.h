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

bool Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len);
bool Handle_C_MOVE(std::shared_ptr<PacketSession>& session, Proto::C_MOVE& pkt);
bool Handle_C_SKILL(std::shared_ptr<PacketSession>& session, Proto::C_SKILL& pkt);
bool Handle_C_PONG(std::shared_ptr<PacketSession>& session, Proto::C_PONG& pkt);

class ClientProtocolManager
{
public:
	static void Init()
	{
		for (__int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_MOVE] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::C_MOVE>(Handle_C_MOVE, session, buffer, len); };
		GPacketHandler[PKT_C_SKILL] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::C_SKILL>(Handle_C_SKILL, session, buffer, len); };
		GPacketHandler[PKT_C_PONG] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len) { return HandlePacket<Proto::C_PONG>(Handle_C_PONG, session, buffer, len); };
	}

	static bool HandlePacket(std::shared_ptr<PacketSession>& session, BYTE* buffer, __int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::S_ENTERGAME& pkt) { return MakeSendBuffer(pkt, PKT_S_ENTERGAME); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::S_LEAVEGAME& pkt) { return MakeSendBuffer(pkt, PKT_S_LEAVEGAME); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::S_SPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_SPAWN); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::S_DESPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_DESPAWN); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::S_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_S_MOVE); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::S_SKILL& pkt) { return MakeSendBuffer(pkt, PKT_S_SKILL); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::S_DIE& pkt) { return MakeSendBuffer(pkt, PKT_S_DIE); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::S_CHANGEHP& pkt) { return MakeSendBuffer(pkt, PKT_S_CHANGEHP); }
	static std::shared_ptr<SendBuffer> MakeSendBuffer(Proto::S_PING& pkt) { return MakeSendBuffer(pkt, PKT_S_PING); }

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
		//ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->SetWriteSize(packetSize);

		return sendBuffer;
	}
};

