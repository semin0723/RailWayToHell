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

	// ���� ��ǥ�� ��ȯ�մϴ�.
	Vector3 GetPosition() const;
	// �θ�κ����� �����ǥ�� ��ȯ�մϴ�.
	Vector3 GetLocalPosition() const;
	void SetPosition(Vector3 vec);
	void SetPosition(float x, float y, float z);
	void SetPositionX(float x);
	void SetPositionY(float y);
	void SetPositionZ(float z);

	// �θ��� ȸ������ ������ ������� ��ȯ�մϴ�.
	Vector3 GetRotation() const;
	// �ڽ��� ȸ������ ��ȯ�մϴ�.
	Vector3 GetLocalRotation() const;
	void SetRotation(Vector3 vec);
	void SetRotation(float x, float y, float z);
	void SetRotationX(float x);
	void SetRotationY(float y);
	void SetRotationZ(float z);

	// �θ��� �����ϱ��� ������ ������� ��ȯ�մϴ�.
	Vector3 GetScale() const;
	// �ڽ��� �����ϸ� ��ȯ�մϴ�.
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

	// ���� ��ǥ�� ���� �����¿� (�θ� ��� x)
	Vector3 Up();
	Vector3 Right();
	Vector3 Down();
	Vector3 Left();

	// Easy Animation
	// destination���� ��� �̵�
	void EasyMove_MoveTo(Vector3 destination, float duration);
	// movement��ŭ ��� �̵�
	void EasyMove_MoveBy(Vector3 movement, float duration);
	// rotation���� ��� ȸ��
	void EasyMove_RotateTo(Vector3 rotation, float duration);
	// rotation��ŭ ��� ȸ��
	void EasyMove_RotateBy(Vector3 rotation, float duration);
	// scale���� ��� ������
	void EasyMove_ScaleTo(Vector3 scale, float duration);
	// scale��ŭ ��� ������
	void EasyMove_ScaleBy(Vector3 scale, float duration);

	// magnitude: �̵� �Ÿ�, duration: ���� �ð�, frequency: �ʴ� �պ� Ƚ��
	// frequency�� 0������ ��� �� 0.05�ʴ� 1ȸ �պ����� ����
	void EasyMove_ShakeHorizontal(float magnitude, float duration, int frequency = 0, bool startToRight = true, EasyMove_Type type = EasyMove_Type::Linear, bool restore_transform_when_done = false);
	// magnitude: �̵� �Ÿ�, duration: ���� �ð�, frequency: �ʴ� �պ� Ƚ��
	// frequency�� 0������ ��� �� 0.05�ʴ� 1ȸ �պ����� ����
	void EasyMove_ShakeVertical(float magnitude, float duration, int frequency = 0, bool startToDown = true, EasyMove_Type type = EasyMove_Type::Linear, bool restore_transform_when_done = false);
	// magnitude: �̵� �Ÿ�, duration: ���� �ð�, frequency: �ʴ� �պ� Ƚ��
	// frequency�� 0������ ��� �� 0.05�ʴ� 1ȸ �պ����� ����
	void EasyMove_ShakeScale(float magnitude, float duration, int frequency = 0, bool startToUpscale = true, EasyMove_Type type = EasyMove_Type::Linear, bool restore_transform_when_done = false);
};