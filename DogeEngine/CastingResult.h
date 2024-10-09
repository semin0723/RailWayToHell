#pragma once
#include <vector>

class Collider;
class Object;

struct CastHit
{
	Collider* collider;
	Object* gameObject;

	// <24-05-08>
	// 아직 교차점 거리 구하기는 못 하겠다. Box-Line은 가능해도 Circle-Line이나 다른 모양은 도저히 엄두가 안 남.
	//float distance;
};
struct CastingResult
{
	std::vector<CastHit> hits;

	// 나중에 distance 기준 정렬 추가
};