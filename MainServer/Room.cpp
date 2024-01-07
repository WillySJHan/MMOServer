#include "stdafx.h"
#include "Room.h"

#include "ClientProtocolManager.h"
#include "Player.h"
#include "ClientSession.h"
#include "Proto.pb.h"
#include "Map.h"
#include "Bullet.h"
#include "ObjectManager.h"
#include "Monster.h"
#include "DataManager.h"
#include "Data.h"
#include "Area.h"
#include "VisualField.h"
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>


Room::Room() : _map(std::make_shared<Map>())
{
}

void Room::Init(int mapId, __int32 areaCells)
{
	_map->LoadMap(mapId);

	_areaCells = areaCells;

	__int32 countY = (_map->SizeY() + areaCells - 1) / areaCells;
	__int32 countX = (_map->SizeX() + areaCells - 1) / areaCells;

	_areas.resize(countY);

	for (__int32 i = 0; i < countY; i++)
	{
		std::vector<std::shared_ptr<Area>> temp(countX);
		_areas[i] = temp;
	}

	for (__int32 y = 0;y<countY;y++)
	{
		for (__int32 x = 0;x<countX;x++)
		{
			_areas[y][x] = std::make_shared<Area>(y,x);
		}
	}

	for (__int32 i = 0;i<50; i++)
	{
		std::shared_ptr<Monster> monster = ObjectManager::Instance()->Add<Monster>();
		//monster->SetCellpos(Vector2Int(5, 5));
		Enter(monster, true);
	}

}

void Room::Update()
{

}

void Room::UpdateProjectile(std::shared_ptr<Projectile> projectile)
{
	projectile->Update();
}

void Room::UpdateMonster(std::shared_ptr<Monster> monster)
{
	monster->Update();
}

void Room::UpdateVisualField(std::shared_ptr<class Player> player)
{


	player->_visualField->Update();
	
}


void Room::PingCheck(std::shared_ptr<Player> player)
{
	if (player->_ownerSession == nullptr)
		return;
	player->_ownerSession->Ping();
}


void Room::Enter(std::shared_ptr<BaseObject> baseObject, bool randomPos)
{
	if (baseObject == nullptr)
		return;

	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// 랜덤 엔진 및 분포 설정
	std::default_random_engine generator(std::rand());
	std::uniform_int_distribution<int> distributionX(_map->_minX, _map->_maxX);
	std::uniform_int_distribution<int> distributionY(_map->_minY, _map->_maxY);

	//int randomValue = distribution(generator);

	if (randomPos)
	{
		Vector2Int respawnPos(0, 0);
		while (true)
		{
			respawnPos.x = distributionX(generator);
			respawnPos.y = distributionY(generator);
			if (_map->Find(respawnPos) == nullptr)
			{
				baseObject->_info->mutable_posinfo()->set_posx(respawnPos.x);
				baseObject->_info->mutable_posinfo()->set_posy(respawnPos.y);
				break;
			}
		}
	}
	



	

	Proto::ObjectType type = ObjectManager::GetObjectTypeById(baseObject->_info->objectid());

	if (type == Proto::PLAYER)
	{
		std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(baseObject);
		if (!player)
			return;
		/////////////
		//player->_visualField = std::make_shared<VisualField>(player);
		
		_players[baseObject->_info->objectid()] = player;

		player->_room = std::static_pointer_cast<Room>(shared_from_this());
		///////////////////////////////////////////////////////
		_map->ApplyMove(player, Vector2Int(player->GetCellPos().x, player->GetCellPos().y));

		GetArea(player->GetCellPos())->_players.insert(player);

		{
			Proto::S_ENTERGAME enterPacket;
			*enterPacket.mutable_player() = *(player->_info);
			auto enterSendBuffer = ClientProtocolManager::MakeSendBuffer(enterPacket);
			player->_ownerSession->Send(enterSendBuffer);


				player->_visualField->Update();
			

		}
	}
	else if (type == Proto::ObjectType::MONSTER)
	{
		std::shared_ptr<Monster> monster = std::dynamic_pointer_cast<Monster>(baseObject);
		if (!monster)
			return;
		_monsters[baseObject->_info->objectid()] = monster;
		monster->_room = std::static_pointer_cast<Room>(shared_from_this());

		GetArea(monster->GetCellPos())->_monsters.insert(monster);
		_map->ApplyMove(monster, Vector2Int(monster->GetCellPos().x, monster->GetCellPos().y));

		monster->Update();
	}
	else if (type == Proto::ObjectType::PROJECTILE)
	{
		std::shared_ptr<Projectile> projectile = std::dynamic_pointer_cast<Projectile>(baseObject);
		if (!projectile)
			return;
		_projectiles[baseObject->_info->objectid()] = projectile;
		projectile->_room = std::static_pointer_cast<Room>(shared_from_this());

		GetArea(projectile->GetCellPos())->_projectiles.insert(projectile);
		projectile->Update();
	}


	{
		Proto::S_SPAWN spawnPacket;
		auto newPlayerInfo = spawnPacket.add_objects();
		*newPlayerInfo = *(baseObject->_info);
		auto spawnSendBuffer = ClientProtocolManager::MakeSendBuffer(spawnPacket);
		Broadcast(baseObject->GetCellPos(), spawnSendBuffer);
	}
		
}

void Room::Leave(__int32 objectId)
{
	//std::lock_guard<std::recursive_mutex> lock(_rMutex);
	Proto::ObjectType type = ObjectManager::GetObjectTypeById(objectId);

	Vector2Int cellpos(0,0);

	if (type == Proto::PLAYER)
	{
		std::shared_ptr<Player> player = _players[objectId];
		if (!player)
			return;

		{
			std::lock_guard<std::recursive_mutex> lock(_rMutex);
			if (!_players.erase(objectId))
				return;
		}

		cellpos = player->GetCellPos();
		//GetArea(player->GetCellPos())->_players.erase(player);

		_map->ApplyLeave(player);
		player->_room = nullptr;

		{

			Proto::S_LEAVEGAME leavePacket;
			auto SendBuffer = ClientProtocolManager::MakeSendBuffer(leavePacket);
			player->_ownerSession->Send(SendBuffer);

		}
		/////////////////////
		if (player->_ownerSession->IsConnected() == false)
			player->_ownerSession = nullptr;
	}
	else if (type == Proto::ObjectType::MONSTER)
	{
		std::shared_ptr<Monster> monster = _monsters[objectId];
		if (!monster)
			return;

		if (!_monsters.erase(objectId))
			return;

		cellpos = monster->GetCellPos();
		//GetArea(monster->GetCellPos())->_monsters.erase(monster);
		_map->ApplyLeave(monster);
		monster->_room = nullptr;
	}

	else if (type == Proto::ObjectType::PROJECTILE)
	{
		std::shared_ptr<Projectile> projectile = _projectiles[objectId];
		if (!projectile)
			return;

		if (!_projectiles.erase(objectId))
			return;

		cellpos = projectile->GetCellPos();
		//GetArea(projectile->GetCellPos())->_projectiles.erase(projectile);
		_map->ApplyLeave(projectile);
		projectile->_room = nullptr;
	}
	else
	{
		return;
	}


	{
		Proto::S_DESPAWN despawnPacket;
		despawnPacket.add_objectids(objectId);
		//despawnPacket.set_objectids(0, objectId);
		auto SendBuffer = ClientProtocolManager::MakeSendBuffer(despawnPacket);

		Broadcast(cellpos, SendBuffer);
	}
}

void Room::Broadcast(Vector2Int pos, std::shared_ptr<SendBuffer> sendBuffer)
{
	std::vector<std::shared_ptr<Area>> areas = GetNearAreas(pos);

	for (auto area : areas)
	{
		for (auto p : area->_players)
		{
			int dx = p->GetCellPos().x - pos.x;
			int dy = p->GetCellPos().y - pos.y;

			if (std::abs(dx) > Room::VisualFieldCells)
				continue;
			if (std::abs(dy) > Room::VisualFieldCells)
				continue;
			//if (!(p->_connected))
			//	return;
			p->_ownerSession->Send(sendBuffer);
		}
	}

	//for (auto& pair : _players)
	//{
	//	pair.second->_ownerSession->Send(sendBuffer);
	//}
}

std::vector<std::shared_ptr<Player>> Room::GetNearPlayers(Vector2Int pos, int searchRange)
{
	std::vector<std::shared_ptr<Area>> areas = GetNearAreas(pos, searchRange);
	std::vector<std::shared_ptr<Player>> players;
	for (auto area : areas)
	{
		for (auto player : area->_players)
		{
			players.push_back(player);
		}
	}
	return players;
}

std::vector<std::shared_ptr<Area>> Room::GetNearAreas(Vector2Int cellPos, __int32 searchRange)
{
	std::unordered_set<std::shared_ptr<Area>> areas;
	__int32 maxY = cellPos.y + searchRange;
	__int32 minY = cellPos.y - searchRange;
	__int32 maxX = cellPos.x + searchRange;
	__int32 minX = cellPos.x - searchRange;

	//좌측상단
	Vector2Int leftTop(minX, maxY);
	int minIndexY = (_map->_maxY - leftTop.y) / _areaCells;
	int minIndexX = (leftTop.x - _map->_minX) / _areaCells;
	//우측하단
	Vector2Int rightBot(maxX, minY);
	int maxIndexY = (_map->_maxY - rightBot.y) / _areaCells;
	int maxIndexX = (rightBot.x - _map->_minX) / _areaCells;

	for (int x = minIndexX; x <= maxIndexX; x++)
	{
		for (int y = minIndexY; y <= maxIndexY; y++)
		{
			std::shared_ptr<Area> area = GetArea(y, x);
			if (area == nullptr)
				continue;

			areas.insert(area);
		}
	}
	std::vector<std::shared_ptr<Area>> vArea(areas.begin(), areas.end());
	return vArea;

}

void Room::ManageMove(std::shared_ptr<Player> player, Proto::C_MOVE pkt)
{
	if (player == nullptr)
		return;
	

		
	Proto::PositionInfo movePosInfo = pkt.posinfo();
	std::shared_ptr<Proto::ObjectInfo> info = player->_info;

	if (movePosInfo.posx() != info->posinfo().posx() || movePosInfo.posy() != info->posinfo().posy())
	{
		if (_map->CanGo(Vector2Int(movePosInfo.posx(), movePosInfo.posy())) == false)
			return;
	}

	info->mutable_posinfo()->set_state(movePosInfo.state());
	info->mutable_posinfo()->set_movedir(movePosInfo.movedir());
	_map->ApplyMove(player, Vector2Int(movePosInfo.posx(), movePosInfo.posy()));


	Proto::S_MOVE resMovePacket;
	resMovePacket.set_objectid(player->_info->objectid());
	*(resMovePacket.mutable_posinfo()) = pkt.posinfo();
	


	auto SendBuffer = ClientProtocolManager::MakeSendBuffer(resMovePacket);
	Broadcast(player->GetCellPos(), SendBuffer);
}

void Room::ManageSkill(std::shared_ptr<Player> player, Proto::C_SKILL pkt)
{
	if (player == nullptr)
		return;

	std::shared_ptr<Proto::ObjectInfo> info = player->_info;
	if (info->posinfo().state() != Proto::CreatureState::IDLE)
		return;

	

	info->mutable_posinfo()->set_state(Proto::CreatureState::SKILL);

	Proto::S_SKILL skill;
	skill.mutable_info();

	skill.set_objectid(info->objectid());
	skill.mutable_info()->set_skillid(pkt.mutable_info()->skillid());

	auto SendBuffer = ClientProtocolManager::MakeSendBuffer(skill);
	Broadcast(player->GetCellPos(), SendBuffer);

	Skill skillData = DataManager::SkillMap[pkt.mutable_info()->skillid()];

	switch (skillData.skillType)
	{
	case Proto::SkillType::SKILL_AUTO:
		{
		Vector2Int skillPos = player->GetFrontCellPos(info->posinfo().movedir());

		std::shared_ptr<BaseObject> target = _map->Find(skillPos);

		if (target != nullptr)
		{
			std::cout << "HitBaseObject" << std::endl;
		}
		}
		break;
	case Proto::SkillType::SKILL_PROJECTILE:
		{
		
		std::shared_ptr<Bullet> bullet = ObjectManager::Instance()->Add<Bullet>();

		if (bullet == nullptr)
			return;

		bullet->_owner = player;
		
		bullet->_info->mutable_posinfo()->set_state(Proto::CreatureState::MOVING);
		*(bullet->_data) = skillData;
		bullet->_info->mutable_posinfo()->set_movedir(player->_info->posinfo().movedir());
		bullet->_info->mutable_posinfo()->set_posx(player->_info->posinfo().posx());
		bullet->_info->mutable_posinfo()->set_posy(player->_info->posinfo().posy());
		bullet->_info->mutable_statinfo()->set_speed(skillData.projectile.speed);
		//Enter(bullet);
		DoAsync((&Room::Enter), std::static_pointer_cast<BaseObject>(bullet),false);
		
		}
		break;
	}



	


}

std::shared_ptr<Player> Room::FindPlayer(std::function<bool(std::shared_ptr<BaseObject>)> condition)
{
	for (const auto& pair : _players)
	{
		if (condition(pair.second))
			return pair.second;
	}

	return nullptr;
}

bool Room::Remove(int objectId)
{
	Proto::ObjectType objectType = ObjectManager::GetObjectTypeById(objectId);
	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);

		if (objectType == Proto::ObjectType::PLAYER)
			return _players.erase(objectId);
	}

	return false;

}

std::shared_ptr<Area> Room::GetArea(Vector2Int cellPos)
{
	__int32 x = (cellPos.x - _map->_minX) / _areaCells;
	__int32 y = (_map->_maxY - cellPos.y) / _areaCells;

	return GetArea(y, x);
}

std::shared_ptr<Area> Room::GetArea(int indexY, int indexX)
{
	if (indexX < 0 || indexX >= _areas[1].size())
		return nullptr;
	if (indexY < 0 || indexY >= _areas.size())
		return nullptr;

	return _areas[indexY][indexX];
}

std::shared_ptr<Player> Room::FindNearPlayer(Vector2Int pos, int searchRange)
{
	std::vector <std::shared_ptr<Player>> players = GetNearPlayers(pos, searchRange);

	std::sort(players.begin(), players.end(), [&](std::shared_ptr<Player> left, std::shared_ptr<Player> right)
		{
			__int32 leftDist = (left->GetCellPos() - pos).GetCellDistFromZero();
			__int32 rightDist = (right->GetCellPos() - pos).GetCellDistFromZero();
			return leftDist < rightDist;
		});

	for (auto player : players)
	{
		std::vector<Vector2Int> path = _map->FindPath(pos, player->GetCellPos(), true);
		if (path.size() < 2 || path.size() > searchRange)
			continue;

		return player;
	}

	return nullptr;
}


