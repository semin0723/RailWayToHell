#pragma once
#include <vector>
#include <string>
#include "CastingResult.h"
#include "Vector2.h"
#include "Vector3.h"

class Rigidbody2D;
class Collider;

class Physics
{
	static std::vector<Collider*> GetColliders(std::vector<std::string> targetTags);
public:
	// �� ���� �浹�ϴ� �ݶ��̴� ��� ��ȯ
	static CastingResult Pointcast(Vector2 point, std::vector<std::string> targetTags = {});
	// �� �������� ���ϴ� ���а� �浹�ϴ� �ݶ��̴� ��� ��ȯ
	static CastingResult Raycast(Vector2 anchor, Vector2 dir, float distance, std::vector<std::string> targetTags = {});

	// Ư�� ��ġ�� ���� �浹�ϴ� �ݶ��̴� ��� ��ȯ
	static CastingResult CircleCast(Vector2 anchor, float radius, std::vector<std::string> targetTags = {});


	// ���� ����
	// �ٸ� ��ü�� �浹
	static Vector3 GetVelocityAfterCollision(Vector3 velocity1, float mass1, float e, Vector3 velocity2, float mass2, Vector3 normal);
	// ������ ������Ʈ�� �浹
	static Vector3 GetVelocityAfterCollision(Vector3 velocity, float mass, float e, Vector3 normal);
};
