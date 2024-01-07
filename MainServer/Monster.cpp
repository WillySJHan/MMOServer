#include "stdafx.h"
#include "Monster.h"

#include "ClientProtocolManager.h"
#include "Data.h"
#include "DataManager.h"
#include "Room.h"
#include "Player.h"

Monster::Monster() : BaseObject(Proto::ObjectType::MONSTER)
{
	SetLevel(1);
	//SetHp(100);
	//_info->mutable_statinfo()->set_hp(100);
	SetMaxHp(100);
	SetHp(100);
	SetSpeed(5.0f);

	SetState(Proto::CreatureState::IDLE);

}

void Monster::Update()
{
	switch (GetState())
	{
	case Proto::CreatureState::IDLE:
		UpdateIdle();
		break;
	case Proto::CreatureState::MOVING:
		UpdateMoving();
		break;
	case Proto::CreatureState::SKILL:
		UpdateSkill();
		break;
	case Proto::CreatureState::DEAD:
		UpdateDead();
		break;
	}

	if (_room != nullptr)
		_task = _room->DoTimer(200, &Room::UpdateMonster, std::static_pointer_cast<Monster>(shared_from_this()));
}

void Monster::BroadcastMove()
{
	Proto::S_MOVE movePacket;

	movePacket.set_objectid(_info->objectid());
	movePacket.mutable_posinfo()->set_posx(GetPosX());
	movePacket.mutable_posinfo()->set_posy(GetPosY());
	movePacket.mutable_posinfo()->set_movedir(GetMoveDir());
	movePacket.mutable_posinfo()->set_state(GetState());

	//movePacket.mutable_posinfo()->CopyFrom(_info->posinfo());
	//movePacket.mutable_posinfo()->MergeFrom(_info->posinfo());

	auto SendBuffer = ClientProtocolManager::MakeSendBuffer(movePacket);
	_room->Broadcast(GetCellPos(), SendBuffer);
}

void Monster::OnDead(std::shared_ptr<BaseObject> attacker)
{
	if (_task != nullptr)
	{
		_task->_cancel = true;
		_task = nullptr;
	}

	BaseObject::OnDead(attacker);


}

void Monster::UpdateIdle()
{
	if (_nextSearchTick > GetTickCount64())
		return;

	_nextSearchTick = GetTickCount64() + 1000;

	std::shared_ptr<Player> target = _room->FindNearPlayer(GetCellPos(), _searchCellDist);
	if (target == nullptr)
		return;

	_target = target;

	SetState(Proto::CreatureState::MOVING);
}

void Monster::UpdateMoving()
{
	if (_nextMoveTick > GetTickCount64())
		return;

	__int32 moveTick = static_cast<int>(1000 / GetSpeed());
	_nextMoveTick = GetTickCount64() + moveTick;

	if (_target == nullptr || _target->_room != _room)
	{
		_target = nullptr;
		SetState(Proto::CreatureState::IDLE);
		BroadcastMove();
		return;
	}

	Vector2Int dir = _target->GetCellPos() - GetCellPos();
	__int32 dist = dir.GetCellDistFromZero();

	if (dist == 0 || dist > _chaseCellDist)
	{
		_target = nullptr;
		SetState(Proto::CreatureState::IDLE);
		BroadcastMove();
		return;
	}

	std::vector<Vector2Int> path = _room->_map->FindPath(GetCellPos(), _target->GetCellPos(), true);
	if (path.size() < 2 || path.size() > _chaseCellDist)
	{
		_target = nullptr;
		SetState(Proto::CreatureState::IDLE);
		BroadcastMove();
		return;
	}

	if (dist <= _skillRange && (dir.x == 0||dir.y ==0))
	{
		_coolTick = 0;
		SetState(Proto::CreatureState::SKILL);

		return;
	}

	SetMoveDir(GetDirFromVec(path[1] - GetCellPos()));
	_room->_map->ApplyMove(shared_from_this(), path[1]);

	BroadcastMove();
	

}

void Monster::UpdateSkill()
{
	if (_coolTick == 0)
	{

		if (_target == nullptr || _target->_room != _room || _target->GetHp() == 0)
		{
			_target = nullptr;
			SetState(Proto::CreatureState::MOVING);
			BroadcastMove();
			return;
		}


		Vector2Int dir = (_target->GetCellPos() - GetCellPos());
		__int32 dist = dir.GetCellDistFromZero();
		bool canUseSkill = (dist <= _skillRange && (dir.x == 0 || dir.y == 0));
		if (canUseSkill == false)
		{
	
			SetState(Proto::CreatureState::MOVING);
			BroadcastMove();
			return;
		}


		Proto::MoveDir lookDir = GetDirFromVec(dir);
		if (GetMoveDir() != lookDir)
		{
			SetMoveDir(lookDir);
			BroadcastMove();
		}

		Skill skillData = DataManager::SkillMap[1];
	
		_target->OnDamaged(shared_from_this(), skillData.damage + _info->mutable_statinfo()->attack());

		Proto::S_SKILL skill;
		skill.set_objectid(_info->objectid());
		skill.mutable_info()->set_skillid(skillData.id);
		auto SendBuffer = ClientProtocolManager::MakeSendBuffer(skill);

		_room->Broadcast(GetCellPos(), SendBuffer);




		__int32 coolTick = static_cast<__int32>(1000 * skillData.cooldown);
		_coolTick = GetTickCount64() + coolTick;

	}

	if (_coolTick > GetTickCount64())
		return;

	_coolTick = 0;
}

void Monster::UpdateDead()
{
}
