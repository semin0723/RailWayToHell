#pragma once
#include <vector>

class Collider;
class Object;

struct CastHit
{
	Collider* collider;
	Object* gameObject;

	// <24-05-08>
	// ���� ������ �Ÿ� ���ϱ�� �� �ϰڴ�. Box-Line�� �����ص� Circle-Line�̳� �ٸ� ����� ������ ���ΰ� �� ��.
	//float distance;
};
struct CastingResult
{
	std::vector<CastHit> hits;

	// ���߿� distance ���� ���� �߰�
};