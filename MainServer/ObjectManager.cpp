#include "stdafx.h"
#include "ObjectManager.h"
#include "Player.h"


bool ObjectManager::Remove(__int32 objectId)
{
	Proto::ObjectType objectType = GetObjectTypeById(objectId);
	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);

		if (objectType == Proto::ObjectType::PLAYER)
			return _players.erase(objectId);
	}

	return false;

	
}

std::shared_ptr<Player> ObjectManager::Find(__int32 objectId)
{
	Proto::ObjectType objectType = GetObjectTypeById(objectId);
	
	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);
		if (objectType == Proto::ObjectType::PLAYER)
		{
			auto it = _players.find(objectId);

			if (it != _players.end()) 
				return it->second;
		}

		
	}
	return nullptr;
}

int ObjectManager::GenerateId(Proto::ObjectType type)
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);

	return ((static_cast<__int32>(type) << 24) | (_counter++));

}
