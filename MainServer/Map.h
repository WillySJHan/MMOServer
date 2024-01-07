#pragma once
#include <cmath>
#include <fstream>
#include <sstream> 

struct Pos
{
    Pos() : Y(0), X(0) {}
    Pos(int y, int x) : Y(y), X(x) {}
    int Y;
	int X;

    bool operator==(const Pos& rhs) const
	{
        return Y == rhs.Y && X == rhs.X;
    }

    bool operator!=(const Pos& rhs) const
	{
        return !(*this == rhs);
    }
};

namespace std
{
    template <>
    struct hash<Pos>
    {
        size_t operator()(const Pos& p) const
        {
            return hash<int>()(p.Y) ^ hash<int>()(p.X);
        }
    };
}


struct PQNode
{
    int F;
    int G;
    int Y;
    int X;

    bool operator<(const PQNode& other) const { return F < other.F; }
    bool operator>(const PQNode& other) const { return F > other.F; }
};




struct Vector2Int
{
    int x;
	int y;
    
    Vector2Int(int x, int y) : x(x), y(y) {}

    static Vector2Int up() { return Vector2Int(0, 1); }
    static Vector2Int down() { return Vector2Int(0, -1); }
    static Vector2Int left() { return Vector2Int(-1, 0); }
    static Vector2Int right() { return Vector2Int(1, 0); }

    Vector2Int operator+(const Vector2Int& a)
	{
        return Vector2Int(x + a.x, y + a.y);
    }

    Vector2Int operator-(const Vector2Int& a)
    {
        return Vector2Int(x - a.x, y - a.y);
    }

    int GetSqrMagnitude() { return (x * x + y * y); }
    float GetMagnitude() { return static_cast<float>(std::sqrt(GetSqrMagnitude())); }
    int GetCellDistFromZero() { return (std::abs(x) + std::abs(y)); }
};

class Map {
public:
    bool CanGo(Vector2Int cellPos, bool checkObjects = true);

    void LoadMap(int mapId, const std::string& pathPrefix = "Map");
    std::shared_ptr<class BaseObject> Find(Vector2Int cellPos);
    bool ApplyMove(std::shared_ptr<class BaseObject> baseObject, Vector2Int dest, bool checkObjects= true, bool collision= true);
    bool ApplyLeave(std::shared_ptr<class BaseObject> baseObject);

    std::vector<Vector2Int> CalcCellPathFromParent(std::unordered_map<Pos,Pos> parent, Pos dest);


    Pos Cell2Pos(Vector2Int cell)
	{
        return Pos(_maxY - cell.y, cell.x - _minX);
    }

    Vector2Int Pos2Cell(Pos pos)
	{
        return Vector2Int(pos.X + _minX, _maxY - pos.Y);
    }

    std::vector<Vector2Int> FindPath(Vector2Int startCellPos, Vector2Int destCellPos, bool checkObjects = true, __int32 maxDist = 10);

public:
    std::vector<std::vector<bool>> _collision;
    std::vector<std::vector<std::shared_ptr<class BaseObject>>> _objects;

    __int32 _minX = 0;
    __int32 _minY = 0;
    __int32 _maxX = 0;
    __int32 _maxY = 0;

    int SizeX() { return _maxX - _minX + 1; }
    int SizeY() { return _maxY - _minY + 1; }




public:
    std::vector<__int32> _deltaY{ 1, -1, 0, 0 };
    std::vector<__int32> _deltaX{ 0, 0, -1, 1 };
    std::vector<__int32> _cost{ 10, 10, 10, 10 };
};