#pragma once

class TagData
{
public:
	std::vector<std::string> tagNames;

	std::map<int, std::vector<bool>> TagCollisionMatrix;

public:
	void AddTag(std::string name);

	void SetTagCollision(int tag1, int tag2, bool collision);

	bool GetTagCollision(int tag1, int tag2);

	int GetTagIndex(std::string name);
};