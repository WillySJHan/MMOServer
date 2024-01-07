#pragma once
#include "Map.h"
#include "Proto.pb.h"

class BaseObject : public std::enable_shared_from_this<BaseObject>
{
public:
	BaseObject();
	BaseObject(Proto::ObjectType objectType);
	virtual ~BaseObject()
	{
		std::cout << "~BaseObject" << std::endl;
	}
	
	virtual void Update();


	Vector2Int GetCellPos();
	void SetCellpos(Vector2Int pos);

	Vector2Int GetFrontCellPos(Proto::MoveDir dir);
	Vector2Int GetFrontCellPos();

	static Proto::MoveDir GetDirFromVec(Vector2Int dir);

	virtual void OnDamaged(std::shared_ptr<BaseObject> attacker, __int32 damage);
	virtual void OnDead(std::shared_ptr<BaseObject> attacker);

	//stat
	__int32 GetLevel() { return _info->mutable_statinfo()->level(); }
	void SetLevel(__int32 level) { _info->mutable_statinfo()->set_level(level); }

	__int32 GetHp() { return _info->mutable_statinfo()->hp(); }
	void SetHp(__int32 hp) { _info->mutable_statinfo()->set_hp(std::clamp(hp,0, _info->mutable_statinfo()->maxhp())); }

	__int32 GetMaxHp() { return _info->mutable_statinfo()->maxhp(); }
	void SetMaxHp(__int32 maxHp) { _info->mutable_statinfo()->set_maxhp(maxHp); }

	float GetSpeed() { return _info->mutable_statinfo()->speed(); }
	void SetSpeed(float speed) { _info->mutable_statinfo()->set_speed(speed); }
	//posinfo

	__int32 GetPosX() { return _info->mutable_posinfo()->posx(); }
	void SetPosX(__int32 posX) { _info->mutable_posinfo()->set_posx(posX); }
	__int32 GetPosY() { return _info->mutable_posinfo()->posy(); }
	void SetPosY(__int32 posY) { _info->mutable_posinfo()->set_posy(posY); }
	Proto::MoveDir GetMoveDir() { return _info->mutable_posinfo()->movedir(); }
	void SetMoveDir(Proto::MoveDir moveDir) { _info->mutable_posinfo()->set_movedir(moveDir); }
	Proto::CreatureState GetState() { return _info->mutable_posinfo()->state(); }
	void SetState(Proto::CreatureState state) { _info->mutable_posinfo()->set_state(state); }

	
public:
	Proto::ObjectType _objectType;
	std::shared_ptr<Proto::ObjectInfo> _info;
	std::shared_ptr<class Room> _room;
	Vector2Int _cellPos;
	
};

