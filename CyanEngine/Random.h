#pragma once
#include <random>

class Random : public Singleton<Random>
{
	static std::default_random_engine dre;
public:
	Random();
	~Random();

	static void Start();

	static int Range(int min, int max);
	static float Range(float min, float max);
};