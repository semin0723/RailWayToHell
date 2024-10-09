#pragma once
#include <unordered_set>
#include "Component.h"
#include "Vector3.h"

enum class EasyMove_Type { Linear, LinearShrink, LinearGrow };

class Transform : public Component
{
	Vector3 position{ 0, 0, 0 };
	Vector3 rotation{ 0, 0, 0 };
	Vector3 scale{ 1, 1, 1 };

	Transform* parent = nullptr;
	std::unordered_set<Transform*> child;
public:
	~Transform();

	// 월드 좌표를 반환합니다.
	Vector3 GetPosition() const;
	// 부모로부터의 상대좌표를 반환합니다.
	Vector3 GetLocalPosition() const;
	void SetPosition(Vector3 vec);
	void SetPosition(float x, float y, float z);
	void SetPositionX(float x);
	void SetPositionY(float y);
	void SetPositionZ(float z);

	// 부모의 회전까지 가산한 결과값을 반환합니다.
	Vector3 GetRotation() const;
	// 자식의 회전값만 반환합니다.
	Vector3 GetLocalRotation() const;
	void SetRotation(Vector3 vec);
	void SetRotation(float x, float y, float z);
	void SetRotationX(float x);
	void SetRotationY(float y);
	void SetRotationZ(float z);

	// 부모의 스케일까지 가산한 결과값을 반환합니다.
	Vector3 GetScale() const;
	// 자식의 스케일만 반환합니다.
	Vector3 GetLocalScale() const;
	void SetScale(Vector3 vec);
	void SetScale(float x, float y, float z);
	void SetScaleX(float x);
	void SetScaleY(float y);
	void SetScaleZ(float z);

	void LookAt(Transform* other);
	void LookAt(Vector3 position);

	Transform* GetParent() const;
	void SetParent(Transform* parent = nullptr);
	std::unordered_set<Transform*>* GetChild();
	void SetChild(Transform* child);

private:
	void MakeRelationship(Transform* _parent, Transform* _child);
public:

	void Translate(Vector3);
	void Translate(float x, float y, float z);
	void Rotate(Vector3);
	void Rotate(float x, float y, float z);

	void ScaleMult(Vector3);
	void ScaleMult(float x, float y, float z);
	void ScaleAdd(Vector3);
	void ScaleAdd(float x, float y, float z);

	// 로컬 좌표계 기준 상하좌우 (부모 고려 x)
	Vector3 Up();
	Vector3 Right();
	Vector3 Down();
	Vector3 Left();

	// Easy Animation
	// destination까지 등속 이동
	void EasyMove_MoveTo(Vector3 destination, float duration);
	// movement만큼 등속 이동
	void EasyMove_MoveBy(Vector3 movement, float duration);
	// rotation까지 등속 회전
	void EasyMove_RotateTo(Vector3 rotation, float duration);
	// rotation만큼 등속 회전
	void EasyMove_RotateBy(Vector3 rotation, float duration);
	// scale까지 등속 스케일
	void EasyMove_ScaleTo(Vector3 scale, float duration);
	// scale만큼 등속 스케일
	void EasyMove_ScaleBy(Vector3 scale, float duration);

	// magnitude: 이동 거리, duration: 지속 시간, frequency: 초당 왕복 횟수
	// frequency가 0이하일 경우 약 0.05초당 1회 왕복으로 설정
	void EasyMove_ShakeHorizontal(float magnitude, float duration, int frequency = 0, bool startToRight = true, EasyMove_Type type = EasyMove_Type::Linear, bool restore_transform_when_done = false);
	// magnitude: 이동 거리, duration: 지속 시간, frequency: 초당 왕복 횟수
	// frequency가 0이하일 경우 약 0.05초당 1회 왕복으로 설정
	void EasyMove_ShakeVertical(float magnitude, float duration, int frequency = 0, bool startToDown = true, EasyMove_Type type = EasyMove_Type::Linear, bool restore_transform_when_done = false);
	// magnitude: 이동 거리, duration: 지속 시간, frequency: 초당 왕복 횟수
	// frequency가 0이하일 경우 약 0.05초당 1회 왕복으로 설정
	void EasyMove_ShakeScale(float magnitude, float duration, int frequency = 0, bool startToUpscale = true, EasyMove_Type type = EasyMove_Type::Linear, bool restore_transform_when_done = false);
};