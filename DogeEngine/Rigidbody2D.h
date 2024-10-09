#pragma once
#include "Component.h"
#include "defines.h"
#include "Vector3.h"

enum class ForceMode { Force, Impulse };
enum class BodyType { Dynamic, Static };

class Object;
class Scnee;
class Collider;
NAMESPACE_DOGEENGINE_BEGIN
class CollisionCheck;
NAMESPACE_DOGEENGINE_END

class Rigidbody2D : public Component
{
	friend class Object;
	friend class Scene;
	friend class DG::CollisionCheck;
	friend class Collider;

	void MoveObject();
	void MoveBackObject();
public:
	~Rigidbody2D() {}

	BodyType bodyType = BodyType::Dynamic;
	float gravityScale = 1.0f;
	// �ӵ� / �ӵ��� ���� �ð� �� �̵� �Ÿ��� �ǹ��ϹǷ�, �̵��� ��ü�� �ӵ���ŭ�� �Ǵ� ���� �´� / AddForce ������ ���� ���� �� ������ ����ؼ� �߰��ϸ� ��
	Vector3 velocity{ 0, 0, 0 };
	// ���ӵ��� �� �ʿ䰡 ������?

	// �ݹ߰�� : �浹 �� ƨ�ܳ����� ����? ���� ���޹��� ���� �ս� ����?
	float restitution_coefficient = 0.0f;
	// ����
	float mass = 1.0f;

	// ����?
	float linear_drag = 0.0f;

	void AddForce(Vector3 force, ForceMode mode = ForceMode::Force);
};