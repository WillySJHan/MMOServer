#pragma once

class RoomManager 
{
public:
    static RoomManager* Instance()
	{
        static RoomManager instance;
        return &instance;
    }

    std::shared_ptr<class Room> Add(int mapId);

    bool Remove(__int32 roomId);

    std::shared_ptr<class Room> Find(__int32 roomId);


private:
    std::recursive_mutex _rMutex;
    std::map<__int32, std::shared_ptr<class Room>> _rooms;
    int _roomId = 1;
};

