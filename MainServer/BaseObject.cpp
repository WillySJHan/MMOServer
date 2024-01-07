#include "stdafx.h"
#include "BaseObject.h"

#include "ClientProtocolManager.h"
#include "Room.h"



BaseObject::BaseObject(Proto::ObjectType objectType) :_objectType(objectType), _info(std::make_shared<Proto::ObjectInfo>()), _cellPos(Vector2Int(0, 0))
{
	_info->mutable_posinfo();
    _info->mutable_statinfo();
}

void BaseObject::Update()
{
}

Vector2Int BaseObject::GetCellPos()
{
    _cellPos.x = _info->posinfo().posx();
    _cellPos.y = _info->posinfo().posy();

    return _cellPos;
}

void BaseObject::SetCellpos(Vector2Int pos)
{
    _info->mutable_posinfo()->set_posx(pos.x);
    _info->mutable_posinfo()->set_posy(pos.y);
}

Vector2Int BaseObject::GetFrontCellPos(Proto::MoveDir dir)
{
    Vector2Int cellPos = GetCellPos();

    switch (dir) {
    case Proto::MoveDir::UP:
        cellPos = cellPos + Vector2Int::up();
        break;
    case Proto::MoveDir::DOWN:
        cellPos = cellPos + Vector2Int::down();
        break;
    case Proto::MoveDir::LEFT:
        cellPos = cellPos + Vector2Int::left();
        break;
    case Proto::MoveDir::RIGHT:
        cellPos = cellPos + Vector2Int::right();
        break;
    }

    return cellPos;
}

Vector2Int BaseObject::GetFrontCellPos()
{
    return GetFrontCellPos(_info->mutable_posinfo()->movedir());
}

Proto::MoveDir BaseObject::GetDirFromVec(Vector2Int dir)
{
    if (dir.x > 0)
        return Proto::MoveDir::RIGHT;
    else if (dir.x < 0)
        return Proto::MoveDir::LEFT;
    else if (dir.y > 0)
        return Proto::MoveDir::UP;
    else
        return Proto::MoveDir::DOWN;
}


void BaseObject::OnDamaged(std::shared_ptr<BaseObject> attacker, int damage)
{
    if (_room == nullptr)
        return;
	_info->mutable_statinfo()->set_hp(std::max(_info->mutable_statinfo()->hp() - damage, 0));

    Proto::S_CHANGEHP changePacket;
    changePacket.set_objectid(_info->objectid());
    changePacket.set_hp(_info->mutable_statinfo()->hp());

    auto SendBuffer = ClientProtocolManager::MakeSendBuffer(changePacket);
    //_room->DoAsync((&Room::Broadcast), SendBuffer);
    _room->Broadcast(GetCellPos(), SendBuffer);

    if (_info->mutable_statinfo()->hp() <= 0)
    {
        //_info->mutable_statinfo()->set_hp(0);
        OnDead(attacker);
    }
}

void BaseObject::OnDead(std::shared_ptr<BaseObject> attacker)
{
    if (_room == nullptr)
        return;
    Proto::S_DIE diePacket;
    diePacket.set_objectid(_info->objectid());
    diePacket.set_attackerid(attacker->_info->objectid());

    auto SendBuffer = ClientProtocolManager::MakeSendBuffer(diePacket);
    //_room->DoAsync((&Room::Broadcast), SendBuffer);
    _room->Broadcast(GetCellPos(), SendBuffer);


    std::shared_ptr<Room> room = _room;
    //room->DoAsync((&Room::Leave), _info->objectid());
    room->Leave(_info->objectid());

    _info->mutable_statinfo()->set_hp(_info->mutable_statinfo()->maxhp());
    _info->mutable_posinfo()->set_state(Proto::CreatureState::IDLE);
    _info->mutable_posinfo()->set_movedir(Proto::MoveDir::DOWN);
    //_info->mutable_posinfo()->set_posx(0);
    //_info->mutable_posinfo()->set_posy(0);

    //room->DoAsync((&Room::Enter), std::static_pointer_cast<BaseObject>(shared_from_this()));
    
    room->Enter(shared_from_this(),true);

}


