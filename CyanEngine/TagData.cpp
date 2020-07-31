#include "pch.h"
#include "TagData.h"

void TagData::AddTag(std::string name)
{
	if (std::find(tagNames.begin(), tagNames.end(), name) != tagNames.end()) return;

	int index = tagNames.size();
	tagNames.push_back(name);

	TagCollisionMatrix[index] = std::vector<bool>(index, false);
	for (auto& tcm : TagCollisionMatrix)
	{
		tcm.second.push_back(false);
	}
}

void TagData::SetTagCollision(int tag1, int tag2, bool collision)
{
	TagCollisionMatrix[tag1][tag2] = collision;
	TagCollisionMatrix[tag2][tag1] = collision;
}

bool TagData::GetTagCollision(int tag1, int tag2)
{
	return TagCollisionMatrix[tag1][tag2];
}

int TagData::GetTagIndex(std::string name)
{
	return std::distance(tagNames.begin(), std::find(tagNames.begin(), tagNames.end(), name));
}