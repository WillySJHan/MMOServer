#include "stdafx.h"
#include "RoomManager.h"
#include "Room.h"

std::shared_ptr<Room> RoomManager::Add(int mapId)
{
	std::shared_ptr<class Room> room = std::make_shared<class Room>();
	//room->Init(mapId);
	room->DoAsync((&Room::Init), mapId, 10);
		
	std::lock_guard<std::recursive_mutex> lock(_rMutex);
	{
		room->SetRoomId(_roomId);
		_rooms[_roomId] = room;
		_roomId++;
	}
	return room;
}

bool RoomManager::Remove(__int32 roomId)
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);

	return _rooms.erase(roomId);
}

std::shared_ptr<Room> RoomManager::Find(__int32 roomId)
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);

	auto it = _rooms.find(roomId);

	if (it != _rooms.end()) {
		return it->second;
	}

	return nullptr;
}
