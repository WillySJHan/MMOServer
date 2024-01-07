#include "stdafx.h"
#include "VisualField.h"

#include "Area.h"
#include "ClientProtocolManager.h"
#include "Player.h"
#include "Room.h"
#include "Monster.h"
#include "Projectile.h"
#include "ClientSession.h"


VisualField::VisualField(std::shared_ptr<Player> owner) : _owner(owner)
{
	//std::cout << "VisualField" << std::endl;
}

VisualField::~VisualField()
{
	std::cout << "~VisualField" << std::endl;
}

std::unordered_set<std::shared_ptr<BaseObject>> VisualField::SurroundingObjects()
{
	std::unordered_set<std::shared_ptr<BaseObject>> objects;
	
	if (auto owner = _owner.lock())
	{
		if (owner == nullptr || owner->_room == nullptr)
		{
			objects.insert(nullptr);
			return objects;
		}

		Vector2Int cellpos = owner->GetCellPos();
		std::vector<std::shared_ptr<Area>> areas = owner->_room->GetNearAreas(cellpos);

		for (auto area : areas)
		{
			for (auto player : area->_players)
			{
				int dx = player->GetCellPos().x - cellpos.x;
				int dy = player->GetCellPos().y - cellpos.y;

				if (std::abs(dx) > Room::VisualFieldCells)
					continue;
				if (std::abs(dy) > Room::VisualFieldCells)
					continue;

				objects.insert(player);
			}
			for (auto monster : area->_monsters)
			{
				int dx = monster->GetCellPos().x - cellpos.x;
				int dy = monster->GetCellPos().y - cellpos.y;

				if (std::abs(dx) > Room::VisualFieldCells)
					continue;
				if (std::abs(dy) > Room::VisualFieldCells)
					continue;

				objects.insert(monster);
			}
			for (auto projectile : area->_projectiles)
			{
				int dx = projectile->GetCellPos().x - cellpos.x;
				int dy = projectile->GetCellPos().y - cellpos.y;

				if (std::abs(dx) > Room::VisualFieldCells)
					continue;
				if (std::abs(dy) > Room::VisualFieldCells)
					continue;

				objects.insert(projectile);
			}
		}
	}

	return objects;
}

void VisualField::Update()
{
	if (auto owner = _owner.lock())
	{
		if (owner == nullptr || owner->_room == nullptr)
			return;




		std::unordered_set <std::shared_ptr<BaseObject>> currentObjects = SurroundingObjects();

		//spawn
		std::unordered_set< std::shared_ptr<BaseObject>> difference(currentObjects.begin(), currentObjects.end());
		
		for (const auto& value : _previousObjects)
		{
			difference.erase(value);
			
		}
		std::vector<std::shared_ptr<BaseObject>> added(difference.begin(), difference.end());
		if (added.size() > 0)
		{
			Proto::S_SPAWN spawnPacket;

			for (auto baseObject : added)
			{
				Proto::ObjectInfo info;
				info.MergeFrom(*baseObject->_info);
				*(spawnPacket.add_objects()) = info;
			}

			auto spawnSendBuffer = ClientProtocolManager::MakeSendBuffer(spawnPacket);
			owner->_ownerSession->Send(spawnSendBuffer);
		}
		//despawn
		std::unordered_set< std::shared_ptr<BaseObject>> difference2(_previousObjects.begin(), _previousObjects.end());
		for (const auto& value : currentObjects)
		{

				difference2.erase(value);
			
		}
		std::vector<std::shared_ptr<BaseObject>> removed(difference2.begin(), difference2.end());
		if (removed.size() > 0)
		{
			Proto::S_DESPAWN despawnPacket;

			for (auto baseObject : removed)
			{
				despawnPacket.add_objectids(baseObject->_info->objectid());
			}
			auto despawnSendBuffer = ClientProtocolManager::MakeSendBuffer(despawnPacket);
			owner->_ownerSession->Send(despawnSendBuffer);
		}

		_previousObjects = currentObjects;


		
		_task = owner->_room->DoTimer(100, &Room::UpdateVisualField, owner);

	}
}
