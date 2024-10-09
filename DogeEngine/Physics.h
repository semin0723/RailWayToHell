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
	// 한 점과 충돌하는 콜라이더 목록 반환
	static CastingResult Pointcast(Vector2 point, std::vector<std::string> targetTags = {});
	// 한 방향으로 향하는 선분과 충돌하는 콜라이더 목록 반환
	static CastingResult Raycast(Vector2 anchor, Vector2 dir, float distance, std::vector<std::string> targetTags = {});

	// 특정 위치의 원과 충돌하는 콜라이더 목록 반환
	static CastingResult CircleCast(Vector2 anchor, float radius, std::vector<std::string> targetTags = {});


	// 고전 물리
	// 다른 강체와 충돌
	static Vector3 GetVelocityAfterCollision(Vector3 velocity1, float mass1, float e, Vector3 velocity2, float mass2, Vector3 normal);
	// 고정된 오브젝트와 충돌
	static Vector3 GetVelocityAfterCollision(Vector3 velocity, float mass, float e, Vector3 normal);
};
