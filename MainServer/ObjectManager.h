#pragma once
#include "BaseObject.h"
#include "Player.h"
class ObjectManager
{
public:
    static ObjectManager* Instance()
    {
        static ObjectManager instance;
        return &instance;
    }

    template <typename T>
    std::shared_ptr<T> Add();
    static Proto::ObjectType GetObjectTypeById(__int32 id)
    {
        __int32 type = (id >> 24) & 0x7F;
        return static_cast<Proto::ObjectType>(type);
    }


    bool Remove(__int32 playerId);

    std::shared_ptr<class Player> Find(__int32 objectId);
    int GenerateId(Proto::ObjectType type);


private:
    std::recursive_mutex _rMutex;
    std::map<__int32, std::shared_ptr<class Player>> _players;
    int _counter = 0; // TODO
};

template <typename T>
std::shared_ptr<T> ObjectManager::Add()
{
    std::shared_ptr<T> object = std::make_shared<T>();
    std::shared_ptr<BaseObject> baseObject = std::dynamic_pointer_cast<BaseObject>(object);
    if (!baseObject)
        return nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(_rMutex);

        baseObject->_info->set_objectid(GenerateId(baseObject->_objectType));

        if (baseObject->_objectType == Proto::ObjectType::PLAYER)
        {
            _players[baseObject->_info->objectid()] = std::static_pointer_cast<Player>(baseObject);
        }

    }
    object = std::static_pointer_cast<T>(baseObject);
    return object;
}

