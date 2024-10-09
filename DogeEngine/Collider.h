#pragma once
#include <unordered_set>
#include <string>
#include "defines.h"
#include "Component.h"
#include "Structures.h"
#include "Vector2.h"
#include "ColliderType.h"

class BoxCollider;
class CircleCollider;
class Rigidbody2D;
class Object;
class Scene;
NAMESPACE_DOGEENGINE_BEGIN
class CollisionCheck;
NAMESPACE_DOGEENGINE_END


// 모든 종류의 콜라이더 찾을 때 Collider 타입으로 찾아야 하므로 네임스페이스로 감싸지 말 것.
class Collider : public Component
{
	friend class Object;
	friend class Scene;
	friend class DG::CollisionCheck;

	ColliderType type;
	Vector2 anchor{ 0, 0 };
	std::unordered_set<Collider*> collided;

	// SimplePhysics에서 밀어낼 때 사용 / 충돌할 경우 최소 overlap을 보장하는 separate axis
	Vector2 bestSeparateAxis;
	float minOverlap;
protected:
	Collider() {}
public:
	// <07-15> 레이어 안 만들고 태그로 거르기? 일단 임시로 이렇게
	std::unordered_set<std::string> ignore_collision_tags;

	bool isTrigger = false;

	virtual ~Collider() {}
	bool IsCollideWith(Collider* _collider);
	bool IsCollideWith(Vector2 point);
	bool IsCollideWith(Vector2 lineStart, Vector2 lineEnd);
	bool IsCollideWithCursor();

	// 분리 축 이론을 사용하여 Rigidbody2D의 movement(현재 위치 - 이전 위치) 방향을 분리 축으로 하여 overlap된 크기를 반환한다. 당연히 Rigidbody2D 컴포넌트를 가진 콜라이더가 기준이 된다.
	float GetOverlapAmountBySATWith(Collider* other, Vector2 moveDir);


	void SetAnchor(float x, float y);
	Vector2 GetAnchor();
	virtual void DrawBounds(Color color) {}

	Vector2 GetCenterPosition();
	ColliderType GetColliderType();
};