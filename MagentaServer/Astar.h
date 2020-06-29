#pragma once
#include <list>
#include <vector>
#include <fstream>
#include <iterator>
#include <windows.h>

struct Pos {
	short x, y;
};

struct tileInform
{
	Pos p;
	int index;
	int parentIndex;
};

typedef std::vector<std::list<tileInform*>>	worldGraph;

class Gameworld
{
private:
	std::vector<tileInform*> tiles;
	worldGraph				myGraph;

public:
	Gameworld();
	~Gameworld();

	worldGraph& getMyGraph();
	std::vector<tileInform*>& getTiles();

	void LoadTile();
	void MakeGraph();
};

class Astar
{
private:
	std::list<int>			openLst;
	std::list<int>			closeLst;
	std::list<tileInform*>	bestLst;	// °æ·Î
	int	startIndex = 0;

public:
	Gameworld* gameworld{ nullptr };

public:
	Astar();
	~Astar();
	void StartAstar(const Pos& startIdx, const Pos& goalIdx);
	std::list<tileInform*>& getBestLst();

private:

	bool PathFinding(int startIdx, int goalIdx);
	void MakePath(int iStartIndex, int iGoalIndex);

	bool CheckOpenLst(int idx);
	bool CheckCloseLst(int idx);
};