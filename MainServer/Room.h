#pragma once
#include "Proto.pb.h"
#include "TaskQueue.h"

class Room : public TaskQueue
{
public:
	Room();

	void Init(int mapId, __int32 areaCells);

	void Update();

	void UpdateProjectile(std::shared_ptr<class Projectile> projectile);
	void UpdateMonster(std::shared_ptr<class Monster> monster);
	void UpdateVisualField(std::shared_ptr<class Player> player);
	//void ClearMemory(std::shared_ptr<class ClientSession> session);

	void PingCheck(std::shared_ptr<class Player> player);

	void Enter(std::shared_ptr<class BaseObject> player, bool randomPos);
	void Leave(__int32 objectId);
	void Broadcast(struct Vector2Int pos,std::shared_ptr<class SendBuffer> sendBuffer);
	std::vector<std::shared_ptr<class Player>> GetNearPlayers(Vector2Int pos, __int32 searchRange);
	std::vector<std::shared_ptr<class Area>> GetNearAreas(struct Vector2Int cellPos, __int32 searchRange = Room::VisualFieldCells);

	void SetRoomId(__int32 roomId) { _roomId = roomId; }
	__int32 GetRoomId() { return _roomId; }

	void ManageMove(std::shared_ptr<class Player> player, Proto::C_MOVE pkt);
	void ManageSkill(std::shared_ptr<class Player> player, Proto::C_SKILL pkt);

	

	// playerªË¡¶
	bool Remove(__int32 objectId);

	std::shared_ptr<class Area> GetArea(struct Vector2Int cellPos);
	std::shared_ptr<class Area> GetArea(__int32 indexY, __int32 indexX);

	std::shared_ptr<class Player> FindNearPlayer(Vector2Int pos, __int32 searchRange);
private:
	std::shared_ptr<class Player> FindPlayer(std::function<bool(std::shared_ptr<BaseObject>)> condition);
	
public:
	__int32 _roomId;
	std::shared_ptr<class Map> _map;

	std::vector<std::vector<std::shared_ptr<class Area>>> _areas;
	__int32 _areaCells;
	static const __int32 VisualFieldCells = 5;

private:
	std::map<__int32, std::shared_ptr<class Player>> _players;
	std::map<__int32, std::shared_ptr<class Monster>> _monsters;
	std::map<__int32, std::shared_ptr<class Projectile>> _projectiles;
	std::recursive_mutex _rMutex;
	
};



