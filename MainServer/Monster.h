#pragma once
#include "BaseObject.h"

class Monster : public BaseObject
{
public:
	Monster();
	~Monster()
	{
		std::cout << "~Monster" << std::endl;
	}
	void Update() override;
	void BroadcastMove();
	void OnDead(std::shared_ptr<BaseObject> attacker) override;
protected:
	virtual void UpdateIdle();
	virtual void UpdateMoving();
	virtual void UpdateSkill();
	virtual void UpdateDead();


public:
	unsigned __int64 _nextSearchTick = 0;
	unsigned __int64 _nextMoveTick = 0;
	__int32 _searchCellDist = 10;
	__int32 _chaseCellDist = 20;
	__int32 _skillRange = 1;
	unsigned __int64 _coolTick = 0;
	std::shared_ptr<class Player> _target;
	std::shared_ptr<class Task> _task;
};

