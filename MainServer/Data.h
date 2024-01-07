#pragma once
#include "Proto.pb.h"




class ProjectileInfo
{
public:
	std::string name;
	float speed;
	__int32 range;
	std::string prefab;
};

class Skill
{
public:
	__int32 id;
	std::string name;
	float cooldown;
	int damage;
	Proto::SkillType skillType;
	ProjectileInfo projectile;
};


