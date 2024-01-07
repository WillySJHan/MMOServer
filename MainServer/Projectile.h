#pragma once
#include "BaseObject.h"
#include "Data.h"


class Projectile : public BaseObject 
{
public:
	Projectile();
	~Projectile()
	{
		std::cout << "~Projectile" << std::endl;
	}

	virtual void Update();

public:
	std::shared_ptr<Skill> _data;
};

