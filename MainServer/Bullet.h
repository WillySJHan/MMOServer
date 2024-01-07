#pragma once
#include "Projectile.h"

class Bullet : public Projectile
{
public:
	~Bullet()
	{
		std::cout << "~Bullet" << std::endl;
	}

	void Update() override;



public:
	std::shared_ptr<BaseObject> _owner;
};

