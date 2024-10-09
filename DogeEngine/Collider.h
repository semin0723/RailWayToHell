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


// ��� ������ �ݶ��̴� ã�� �� Collider Ÿ������ ã�ƾ� �ϹǷ� ���ӽ����̽��� ������ �� ��.
class Collider : public Component
{
	friend class Object;
	friend class Scene;
	friend class DG::CollisionCheck;

	ColliderType type;
	Vector2 anchor{ 0, 0 };
	std::unordered_set<Collider*> collided;

	// SimplePhysics���� �о �� ��� / �浹�� ��� �ּ� overlap�� �����ϴ� separate axis
	Vector2 bestSeparateAxis;
	float minOverlap;
protected:
	Collider() {}
public:
	// <07-15> ���̾� �� ����� �±׷� �Ÿ���? �ϴ� �ӽ÷� �̷���
	std::unordered_set<std::string> ignore_collision_tags;

	bool isTrigger = false;

	virtual ~Collider() {}
	bool IsCollideWith(Collider* _collider);
	bool IsCollideWith(Vector2 point);
	bool IsCollideWith(Vector2 lineStart, Vector2 lineEnd);
	bool IsCollideWithCursor();

	// �и� �� �̷��� ����Ͽ� Rigidbody2D�� movement(���� ��ġ - ���� ��ġ) ������ �и� ������ �Ͽ� overlap�� ũ�⸦ ��ȯ�Ѵ�. �翬�� Rigidbody2D ������Ʈ�� ���� �ݶ��̴��� ������ �ȴ�.
	float GetOverlapAmountBySATWith(Collider* other, Vector2 moveDir);


	void SetAnchor(float x, float y);
	Vector2 GetAnchor();
	virtual void DrawBounds(Color color) {}

	Vector2 GetCenterPosition();
	ColliderType GetColliderType();
};