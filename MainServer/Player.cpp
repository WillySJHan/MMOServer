#include "stdafx.h"
#include "Player.h"

#include "VisualField.h"

Player::Player() : BaseObject(Proto::ObjectType::PLAYER)
{
	//_info->mutable_statinfo()->set_speed(20.0f);
	
}

void Player::OnDamaged(std::shared_ptr<BaseObject> attacker, int damage)
{
	BaseObject::OnDamaged(attacker, damage);
}

void Player::OnDead(std::shared_ptr<BaseObject> attacker)
{
	BaseObject::OnDead(attacker);
}




