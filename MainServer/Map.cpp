#include "stdafx.h"
#include "Map.h"

#include "Area.h"
#include "ObjectManager.h"
#include "Room.h"
#include "Player.h"
#include "Struct.pb.h"
#include "Projectile.h"
#include "Monster.h"

bool Map::CanGo(Vector2Int cellPos, bool checkObjects)
{
	if (cellPos.x < _minX || cellPos.x > _maxX)
		return false;
	if (cellPos.y < _minY || cellPos.y > _maxY)
		return false;

	int x = cellPos.x - _minX;
	int y = _maxY - cellPos.y;
	return (!_collision[y][x] && (!checkObjects || _objects[y][x] == nullptr));
}

void Map::LoadMap(int mapId, const std::string& pathPrefix)
{
	std::string a = pathPrefix + "/Map_" + std::string(3 - std::to_string(mapId).length(), '0') + std::to_string(mapId) + ".txt";
	std::ifstream file(a);
	if (!file)
	{
		return;
	}
	file >> _minX >> _maxX >> _minY >> _maxY;

	int xCount = _maxX - _minX + 1;
	int yCount = _maxY - _minY + 1;
	_collision.resize(yCount);
	_objects.resize(yCount);
	for (int i = 0; i < yCount; ++i)
	{
		std::vector<bool> temp(xCount);
		_collision[i] = temp;
	}
	for (int i = 0; i < yCount; ++i)
	{
		std::vector<std::shared_ptr<BaseObject>> temp(xCount);
		_objects[i] = temp;
	}

	for (int y = 0; y < yCount; ++y) {
		std::string line;
		file >> line;
		for (int x = 0; x < xCount; ++x) {
			_collision[y][x] = (line[x] == '1' ? true : false);
		}
	}
}

std::shared_ptr<BaseObject> Map::Find(Vector2Int cellPos)
{
	if (cellPos.x < _minX || cellPos.x > _maxX)
		return nullptr;
	if (cellPos.y < _minY || cellPos.y > _maxY)
		return nullptr;

	int x = cellPos.x - _minX;
	int y = _maxY - cellPos.y;
	return _objects[y][x];

}

bool Map::ApplyMove(std::shared_ptr<BaseObject> baseObject, Vector2Int dest, bool checkObjects, bool collision)
{
	//ApplyLeave(baseObject);



	if (baseObject->_room == nullptr)
		return false;
	if (baseObject->_room->_map.get() != this)
		return false;

	Proto::PositionInfo* posInfo = baseObject->_info->mutable_posinfo();

	if (CanGo(dest, checkObjects) == false)
		return false;

	if (collision)
	{
		{
			int x = posInfo->posx() - _minX;
			int y = _maxY - posInfo->posy();

			if (_objects[y][x] == baseObject)
				_objects[y][x] = nullptr;
		}
		{
			int x = dest.x - _minX;
			int y = _maxY - dest.y;
			_objects[y][x] = baseObject;
		}
	}
	Proto::ObjectType type = ObjectManager::GetObjectTypeById(baseObject->_info->objectid());
	if (type == Proto::ObjectType::PLAYER)
	{
		std::shared_ptr<Player> p = std::static_pointer_cast<Player>(baseObject);

		std::shared_ptr<Area> now = baseObject->_room->GetArea(baseObject->GetCellPos());
		std::shared_ptr<Area> after = baseObject->_room->GetArea(dest);
		if (now != after)
		{
			//if (now != nullptr)
				now->_players.erase(p);
			//if (after != nullptr)
				after->_players.insert(p);
		}

	}
	else if (type == Proto::ObjectType::MONSTER)
	{
		std::shared_ptr<Monster> m = std::static_pointer_cast<Monster>(baseObject);

		std::shared_ptr<Area> now = baseObject->_room->GetArea(baseObject->GetCellPos());
		std::shared_ptr<Area> after = baseObject->_room->GetArea(dest);
		if (now != after)
		{
			//if (now != nullptr)
			now->_monsters.erase(m);
			//if (after != nullptr)
			after->_monsters.insert(m);
		}
	}
	else if (type == Proto::ObjectType::PROJECTILE)
	{
		std::shared_ptr<Projectile> p = std::static_pointer_cast<Projectile>(baseObject);

		std::shared_ptr<Area> now = baseObject->_room->GetArea(baseObject->GetCellPos());
		std::shared_ptr<Area> after = baseObject->_room->GetArea(dest);
		if (now != after)
		{
			//if (now != nullptr)
			now->_projectiles.erase(p);
			//if (after != nullptr)
			after->_projectiles.insert(p);
		}
	}





	//실제 좌표이동
	posInfo->set_posx(dest.x);
	posInfo->set_posy(dest.y);

	return true;
}

bool Map::ApplyLeave(std::shared_ptr<BaseObject> baseObject)
{
	if (baseObject->_room == nullptr)
		return false;
	if (baseObject->_room->_map.get() != this)
		return false;


	Proto::PositionInfo* posInfo = baseObject->_info->mutable_posinfo();

	if (posInfo->posx() < _minX || posInfo->posx() > _maxX)
		return false;
	if (posInfo->posy() < _minY || posInfo->posy() > _maxY)
		return false;

	//Area

	std::shared_ptr<Area> area = baseObject->_room->GetArea(baseObject->GetCellPos());
	area->Remove(baseObject);


	{
		int x = posInfo->posx() - _minX;
		int y = _maxY - posInfo->posy();

		if (_objects[y][x] == baseObject)
			_objects[y][x] = nullptr;
	}

	return true;
}

std::vector<Vector2Int> Map::CalcCellPathFromParent(std::unordered_map<Pos, Pos> parent, Pos dest)
{
	std::vector<Vector2Int> cells;

	if (parent.find(dest) == parent.end())
	{
		Pos best;
		int bestDist = INT32_MAX;

		for (auto value : parent)
		{
			auto pos = value.first;
			int dist = std::abs(dest.X - pos.X) + std::abs(dest.Y - pos.Y);
			// 제일 우수한 후보를 뽑는다
			if (dist < bestDist)
			{
				best = pos;
				bestDist = dist;
			}
		}


		dest = best;
	}

	Pos pos = dest;
	while (parent[pos] != pos)
	{
		cells.push_back(Pos2Cell(pos));
		pos = parent[pos];
	}
	cells.push_back(Pos2Cell(pos));
	std::reverse(cells.begin(), cells.end());

	return cells;
}

std::vector<Vector2Int> Map::FindPath(Vector2Int startCellPos, Vector2Int destCellPos, bool checkObjects, __int32 maxDist)
{
	std::vector<Pos> path;

	
	std::unordered_set<Pos> closeList;
	

	std::unordered_map<Pos, __int32> openList;




	

	std::unordered_map<Pos, Pos> parent;

	std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pq;


	// CellPos -> ArrayPos
	Pos pos = Cell2Pos(startCellPos);
	Pos dest = Cell2Pos(destCellPos);

	openList[pos] = 10 * (std::abs(dest.Y - pos.Y) + std::abs(dest.X - pos.X));

	pq.push(PQNode{ 10 * (std::abs(dest.Y - pos.Y) + std::abs(dest.X - pos.X)),0,pos.Y,pos.X });

	parent[pos] = pos;

	while (pq.empty() == false)
	{

		PQNode pqNode = pq.top();
		pq.pop();

		Pos node{ pqNode.Y,pqNode.X };

		if (closeList.find(node) != closeList.end())
			continue;


		closeList.insert(node);

		if (node.Y == dest.Y && node.X == dest.X)
			break;

		for (int i = 0; i < _deltaY.size(); i++)
		{
			Pos next = Pos{ node.Y + _deltaY[i], node.X + _deltaX[i] };


			if (std::abs(pos.Y-next.Y) + std::abs(pos.X-next.X) > maxDist)
				continue;

			if (next.Y != dest.Y || next.X != dest.X)
			{
				if (CanGo(Pos2Cell(next), checkObjects) == false) // CellPos
					continue;
			}


			if (closeList.find(next) != closeList.end())
				continue;


			int g = 0;
			int h = 10 * ((dest.Y - next.Y) * (dest.Y - next.Y) + (dest.X - next.X) * (dest.X - next.X));
	

			int value = 0;
			auto it = openList.find(next);

			if (it == openList.end())
			{
				value = INT32_MAX;
			}
			else
			{
				value = it->second;
			}

			if (value < g + h)
				continue;


			auto result = openList.insert({ next, g + h });

			if (!result.second) {
				openList[next] = g + h;
			}
			pq.push(PQNode{ g + h,g,next.Y,next.X });

			auto result2 = parent.insert({ next, node });

			if (!result2.second)
			{
				parent[next] = node;
			}
		}
	}

	return CalcCellPathFromParent(parent, dest);
}


