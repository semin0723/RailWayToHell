#include <random>
#include "Random.h"



int Random::Range(int a, int b)
{
	static std::random_device rd;
	static std::mt19937 gen;
	static std::uniform_int_distribution<> dis;

	std::mt19937 genTmp(rd());
	gen = genTmp;
	std::uniform_int_distribution<> disTmp(a, b);
	dis = disTmp;

	return dis(gen);
}

float Random::Range(float a, float b)
{
	int integer = Range((int)(a * 10000), (int)(b * 10000));
	return (float)(integer / 10000.0f);
}
