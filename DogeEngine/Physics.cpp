#include "Physics.h"
#include "Collider.h"
#include "Scene.h"
#include "Core.h"
#include "CollisionCheck.h"
#include "Math.h"

std::vector<Collider*> Physics::GetColliders(std::vector<std::string> targetTags)
{
	std::vector<Collider*> colliders;

	Scene* currentScene = DG::Core::GetInstance().GetCurrentScene();
	auto& objects = currentScene->GetObjectList();

	for (auto obj : objects)
	{
		if (obj->GetEnable() == false || obj->GetObjectLayer() != ObjectLayer::Object) continue;

		if (!targetTags.empty())
		{
			bool tagContain = false;
			for (auto tag : targetTags)
			{
				tagContain = obj->CompareTag(tag);
				if (tagContain == true) break;
			}
			if (tagContain == false) continue;
		}

		auto colliderList = obj->GetAllColliders();
		for (auto collider : colliderList)
		{
			if (collider->enable == true) colliders.push_back(collider);
		}
	}

	return colliders;
}
CastingResult Physics::Pointcast(Vector2 point, std::vector<std::string> targetTags)
{
	std::vector<Collider*> colliders = GetColliders(targetTags);

	CastingResult result;
	for (int i = 0; i < colliders.size(); i++)
	{
		if (DG::CollisionCheck::CheckBetweenColliderAndPoint(colliders[i], point))
		{
			CastHit hit;
			hit.collider = colliders[i];
			hit.gameObject = colliders[i]->GetOwner();
			result.hits.push_back(hit);
		}
	}

	return result;
}
CastingResult Physics::Raycast(Vector2 anchor, Vector2 dir, float distance, std::vector<std::string> targetTags)
{
	std::vector<Collider*> colliders = GetColliders(targetTags);

	CastingResult result;
	Vector2 lineStart = anchor;
	Vector2 lineEnd = anchor + (dir.Normalize() * distance);
	for (int i = 0; i < colliders.size(); i++)
	{
		if (DG::CollisionCheck::CheckBetweenColliderAndLine(colliders[i], lineStart, lineEnd))
		{
			CastHit hit;
			hit.collider = colliders[i];
			hit.gameObject = colliders[i]->GetOwner();
			result.hits.push_back(hit);
		}
	}

	return result;
}

CastingResult Physics::CircleCast(Vector2 anchor, float radius, std::vector<std::string> targetTags)
{
	std::vector<Collider*> colliders = GetColliders(targetTags);

	CastingResult result;
	for (int i = 0; i < colliders.size(); i++)
	{
		if (DG::CollisionCheck::CheckBetweenColliderAndCircle(colliders[i], anchor, radius))
		{
			CastHit hit;
			hit.collider = colliders[i];
			hit.gameObject = colliders[i]->GetOwner();
			result.hits.push_back(hit);
		}
	}

	return result;
}


// <07-15> 물리 지식이 없어서 구현 못하겠음. 일단 충돌 각도에 따라 분기하는 야매 충돌
Vector3 Physics::GetVelocityAfterCollision(Vector3 v1, float m1, float e, Vector3 v2, float m2, Vector3 normal)
{
	float speed1 = v1.Magnitude();
	float speed2 = v2.Magnitude();
	//float speedTemp = (mass1 * speed1 + mass2 * speed2) / (mass1 + mass2) + (e * (mass2 / (mass1 + mass2)) * (speed2 - speed1));

	float speedTemp = ((m1 - m2) * speed1 + 2 * m2 * speed2) / (m1 + m2);


	float dot = v1.DotProduct(normal);
	if (dot < 0)
	{
		return v1.Reflect(normal).Normalize() * speedTemp * e;
	}
	else
	{
		float radSum = atan2f(v1.y, v1.x) + atan2f(normal.y, normal.x);
		if (radSum - Math::PI >= 0.f) radSum -= Math::PI;
		float average = radSum / 2.f;
		Vector3 dir = Vector3::Right().Rotate(-average).Normalize() * speedTemp * e;
		//return v1.Normalize() * speedTemp * e;
	}
}
Vector3 Physics::GetVelocityAfterCollision(Vector3 velocity, float mass, float e, Vector3 normal)
{
	return velocity.Reflect(normal) * e;
}