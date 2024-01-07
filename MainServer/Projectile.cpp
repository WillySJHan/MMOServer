#include "stdafx.h"
#include "Projectile.h"

Projectile::Projectile() : BaseObject(Proto::ObjectType::PROJECTILE), _data(std::make_shared<Skill>())
{
}

void Projectile::Update()
{
}
