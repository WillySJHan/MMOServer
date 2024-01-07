#pragma once
#include "BaseObject.h"
#include "Proto.pb.h"


class Player : public BaseObject
{
public:
	Player();
	~Player()
	{
		std::cout << "~Player" << std::endl;
	}

	void OnDamaged(std::shared_ptr<BaseObject> attacker, __int32 damage) override;
	virtual void OnDead(std::shared_ptr<BaseObject> attacker) override;
	


public:
	std::shared_ptr<class ClientSession>	_ownerSession; // Cycle
	std::shared_ptr<class VisualField>		_visualField;
	
	
};

