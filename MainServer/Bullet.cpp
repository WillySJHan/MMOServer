#include "stdafx.h"
#include "Bullet.h"

#include "ClientProtocolManager.h"
#include "Room.h"

void Bullet::Update()
{
	if (_data == nullptr|| _owner == nullptr || _room == nullptr)
		return;


	__int64 tick = static_cast<__int64>(1000 / _data->projectile.speed);
	_room->DoTimer(tick, &Room::UpdateProjectile,std::static_pointer_cast<Projectile>(shared_from_this()));
	


	Vector2Int destPos = GetFrontCellPos();

	if (_room->_map->ApplyMove(shared_from_this(), destPos,true, false))
	{

		Proto::S_MOVE movePacket;

		movePacket.set_objectid(_info->objectid());
		*(movePacket.mutable_posinfo()) = _info->posinfo();
		auto SendBuffer = ClientProtocolManager::MakeSendBuffer(movePacket);

		_room->Broadcast(GetCellPos(), SendBuffer);

	}
	else
	{
		std::shared_ptr<BaseObject> target = _room->_map->Find(destPos);
		if (target !=nullptr)
		{
	
			
			target->OnDamaged(std::static_pointer_cast<Bullet>(BaseObject::shared_from_this()),_data->damage+_owner->_info->mutable_statinfo()->attack());
			//std::cout << _data->damage << std::endl;
		}
		// ¼Ò¸ê
		_room->DoAsync((&Room::Leave),_info->objectid());
	}
}
