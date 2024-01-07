#pragma once
class Area
{
public:
	Area() = default;
	Area(__int32 y, __int32 x);

	std::shared_ptr<class Player> FindPlayer(std::function<bool(std::shared_ptr<class Player>)> condition);
	std::vector<std::shared_ptr<class Player>> FindAllPlayers(std::function<bool(std::shared_ptr<class Player>)> condition);
	void Remove(std::shared_ptr<class BaseObject> baseObject);
public:
	__int32 _indexY = 0;
	__int32 _indexX = 0;
	std::unordered_set < std::shared_ptr<class Player>> _players;
	std::unordered_set < std::shared_ptr<class Monster>> _monsters;
	std::unordered_set < std::shared_ptr<class Projectile>> _projectiles;
};

