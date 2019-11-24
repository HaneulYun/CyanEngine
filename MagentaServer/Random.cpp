#include "pch.h"
#include "Random.h"
#include <chrono>

std::default_random_engine Random::dre;

Random::Random()
{
}

Random::~Random()
{
}

void Random::Start()
{
	dre.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

int Random::Range(int min, int max)
{
	std::uniform_int_distribution<int> uid(min, max);
	return uid(dre);
}

float Random::Range(float min, float max)
{
	std::uniform_real_distribution<float> uid(min, max);
	return uid(dre);
}
