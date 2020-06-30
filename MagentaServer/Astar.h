#pragma once
#include <list>

class Node
{
private:
    int x, y;

public:
    Node* parentNode = nullptr;
    bool obstacle;

    int cost = 0;
    int fromStartCost = 0;
    int disttoEnd = 0;

public:
    Node() {}
    Node(int xpos, int ypos) { x = xpos; y = ypos; }
    int getX() { return x; }
    int getY() { return y; }

    void setCoord(int xpos, int ypos)
    {
        x = xpos, y = ypos;
    }

    int distance(const Node& n)
    {
        int newX = x - n.x;
        int newY = y - n.y;
        return abs(newX) + abs(newY);
    }

    Node operator+(const Node& n)
    {
        Node newn;
        newn.x = x + n.x;
        newn.y = y + n.y;
        return newn;
    }

    bool operator==(const Node& n)
    {
        return x == n.x && y == n.y;
    }

    bool operator!=(const Node& n)
    {
        return x != n.x || y != n.y;
    }

    constexpr bool operator <(const Node& left) const
    {
        return (cost > left.cost);
    }

    void ResetCost()
    {
        cost = 0;
        fromStartCost = 0;
        disttoEnd = 0;
    }

    void SetCoast(int g, int h)
    {
        cost = g + h;
        fromStartCost = g;
        disttoEnd = h;
    }
};

class GameWorld
{
public:
   Node* nodeMap[800][800];

public:

    bool is_wall(int x, int y)
    {
        if (x == 0 || x == 799 || y == 0 || y == 799)
            return true;
        return false;
    }
};

class Astar {
private:
    Node* startNode;
    Node* endNode;

public:
    GameWorld* gameworld;
    std::list<Node*> openNodeList;
    std::list<Node*> closeNodeList;
    std::list<Node*> path;

    Node* offsetCoords[4]
    {
       new Node(-1, 0), new Node(1, 0), new Node(0, -1), new Node(0, 1)
    };

public:
    bool PathFinding(int sx, int sy, int ex, int ey);
    bool AddOpenNodeWithNextNode(Node* nextNode);
    void CalcCoast(Node* nextNode, Node* endNode);
    Node* GetNextNode();
};