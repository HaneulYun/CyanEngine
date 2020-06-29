#include "Astar.h"

Gameworld::Gameworld()
{
	LoadTile();
	MakeGraph();
}

Gameworld::~Gameworld()
{

}

worldGraph& Gameworld::getMyGraph()
{
	return myGraph;
}
std::vector<tileInform*>& Gameworld::getTiles()
{
	return tiles;
}

void Gameworld::LoadTile()
{
	std::ifstream in{ "astar.txt" };
	std::istream_iterator<int> a_i(in);

	tileInform* tinfo = nullptr;
	int index = 0;
	tiles.reserve(800 * 800);

	while (a_i != std::istream_iterator<int>())
	{
		tinfo = new tileInform;
		ZeroMemory(tinfo, sizeof(tileInform));

		tinfo->p.x = *a_i % 800;
		tinfo->p.y = *a_i / 800;
		tinfo->index = index;

		tiles.push_back(tinfo);
		index++;
		*a_i++;
	}
}

void  Gameworld::MakeGraph()
{
	myGraph.resize(800 * 800);
	int index = 0;

	for (int i = 0; i < 800; ++i) {
		for (int j = 0; j < 800; ++j) {
			index = j + 800 * i;
			if (index >= tiles.size())
				return;
			if ((0 != i) && (0 != index % (800 * 2)))
			{
				if (0 != (i % 2))
					myGraph[index].push_back(tiles[index - 800]);
				else if (0 == (i % 2))
					myGraph[index].push_back(tiles[index - 801]);
			}

			if ((0 != i) && ((800 * 2 - 1) != index % (800 * 2)))
			{
				if (0 != (i % 2))
					myGraph[index].push_back(tiles[index - 799]);
				else if (0 == (i % 2))
					myGraph[index].push_back(tiles[index - 800]);
			}

			if ((800 - 1 != i) && (0 != index % (800 * 2)))
			{
				if (0 != (i % 2))
					myGraph[index].push_back(tiles[index + 800]);
				else if (0 == (i % 2))
					myGraph[index].push_back(tiles[index + 799]);
			}

			if ((800 - 1 != i) && ((800 * 2 - 1) != index % (800 * 2)))
			{
				if (0 != (i % 2))
					myGraph[index].push_back(tiles[index + 801]);
				else if (0 == (i % 2))
					myGraph[index].push_back(tiles[index + 800]);
			}
		}
	}
}

Astar::Astar()
{
}

Astar::~Astar()
{
}

std::list<tileInform*>& Astar::getBestLst() { return bestLst; }

void Astar::StartAstar(const Pos& startIdx, const Pos& goalIdx)
{
	openLst.clear();
	closeLst.clear();
	bestLst.clear();

	startIndex = startIdx.x + (startIdx.y * 800);
	int goalIndex = goalIdx.x + (goalIdx.y * 800);

	if (startIndex == goalIndex)
		return;

	if (true == PathFinding(startIndex, goalIndex))
		MakePath(startIndex, goalIndex);
}

bool Astar::PathFinding(int startIdx, int goalIdx)
{
	worldGraph& wgraph = gameworld->getMyGraph();
	if (!openLst.empty())
		openLst.pop_front();

	closeLst.push_back(startIdx);

	for (auto& tile : wgraph[startIdx])
	{
		if (goalIdx == tile->index) {
			tile->parentIndex = startIdx;
			return true;
		}

		if (CheckOpenLst(tile->index) || CheckCloseLst(tile->index))
			continue;

		tile->parentIndex = startIdx;
		openLst.push_back(tile->index);
	}

	if (openLst.empty())
		return false;

	const std::vector<tileInform*>& tiles = gameworld->getTiles();
	int orgStart = startIndex;

	openLst.sort([&tiles, &orgStart, &goalIdx](int a, int b)
		{
			Pos d1{ tiles[orgStart]->p.x - tiles[a]->p.x, tiles[orgStart]->p.y - tiles[a]->p.y };
			Pos d2{ tiles[goalIdx]->p.x - tiles[a]->p.x, tiles[goalIdx]->p.y - tiles[a]->p.y };
			Pos d3{ tiles[orgStart]->p.x - tiles[b]->p.x, tiles[orgStart]->p.y - tiles[b]->p.y };
			Pos d4{ tiles[goalIdx]->p.x - tiles[b]->p.x, tiles[goalIdx]->p.y - tiles[b]->p.y };

			float dst1 = sqrt(pow(d1.x, 2) + pow(d1.y, 2)) + sqrt(pow(d2.x, 2) + pow(d2.y, 2));
			float dst2 = sqrt(pow(d3.x, 2) + pow(d3.y, 2)) + sqrt(pow(d4.x, 2) + pow(d4.y, 2));

			return dst1 < dst2;
		});

	return PathFinding(openLst.front(), goalIdx);
}

void Astar::MakePath(int iStartIndex, int iGoalIndex)
{
	const std::vector<tileInform*>& tiles = gameworld->getTiles();
	bestLst.push_front(tiles[iGoalIndex]);
	int iParentIndex = tiles[iGoalIndex]->parentIndex;

	while (true)
	{
		if (iStartIndex == iParentIndex)
			break;

		bestLst.push_front(tiles[iParentIndex]);
		iParentIndex = tiles[iParentIndex]->parentIndex;
	}
}

bool Astar::CheckOpenLst(int idx)
{
	auto iter = std::find(openLst.begin(), openLst.end(), idx);

	if (openLst.end() == iter)
		return false;

	return true;
}

bool Astar::CheckCloseLst(int idx)
{
	auto iter = std::find(closeLst.begin(), closeLst.end(), idx);

	if (closeLst.end() == iter)
		return false;

	return true;
}