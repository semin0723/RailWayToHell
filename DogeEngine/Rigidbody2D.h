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
	// 속도 / 속도는 단위 시간 당 이동 거리를 의미하므로, 이동량 자체는 속도만큼이 되는 것이 맞다 / AddForce 등으로 힘을 가할 때 질량을 고려해서 추가하면 됨
	Vector3 velocity{ 0, 0, 0 };
	// 가속도를 둘 필요가 있을까?

	// 반발계수 : 충돌 후 튕겨나가는 정도? 힘을 전달받을 때의 손실 정도?
	float restitution_coefficient = 0.0f;
	// 질량
	float mass = 1.0f;

	// 마찰?
	float linear_drag = 0.0f;

	void AddForce(Vector3 force, ForceMode mode = ForceMode::Force);
};