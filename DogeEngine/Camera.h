#pragma once
#include "Component.h"
#include "Vector2.h"
#include "Structures.h"

class Transform;

class Camera : public Component
{
	float rectRatio;
	RectF rect;
public:
	float width;
	float height;
	float scale = 1.0f;
	float rotation = 0.0f; // degree

	Camera();
	Transform* GetTransform();

	void ChangeRect(RectF clientRect);
	void ChangeRect(float width, float height);
	// ī�޶��� �ʺ�, ���̸� ���� RECT�� ��ȯ�մϴ�.
	RectF GetScreenRect();
	// ī�޶� ��� ������ ���� �ϴ� �������� ������ǥ�� ��ȯ�մϴ�.
	Vector2 GetCameraMinPosition();
	// ī�޶� ��� ������ ���� ��� �������� ������ǥ�� ��ȯ�մϴ�.
	Vector2 GetCameraMaxPosition();

	// ����� ���� �δ� �� �� �̻��ѵ�, �׳� ī�޶� ������Ʈ�� scale���� ���ְ� BeglongedObject�� �����ϰ��� �̿��ϴ� �� ���� ���� ����
	void EasyMove_ShakeScale(float magnitude, float duration, int frequency = 0, bool startToUpscale = true, bool shrinkMagnitude = true, bool restore_transform_when_done = false);

	static Camera* GetMainCamera();

	// <08-06> View(Ŭ���̾�Ʈ), Screen(�߾��� 0, 0�̰� �¿�� Ŭ���̾�Ʈ ũ�⸸ŭ), World �������� // Screen ��ǥ�� �׳� �»�� (0, 0) Ŭ���̾�Ʈ ��ǥ ������ ��ǥ�� �̸� ��Ȯ�ϰ� �����ؼ� ��ȯ�ϴ� �Լ� ���� ������� ����
	static Vector2 ScreenToWorldPoint(Vector2 point);
	static Vector2 ScreenToWorldPoint(float x, float y);

	static Vector2 WorldToScreenPoint(Vector2 point);
	static Vector2 WorldToScreenPoint(float x, float y);

	// ȭ���� ���� �ϴ��� (0%, 0%)�� ��´� / UI ������Ʈ�� ��ġ�� ������ �� ���. UI ������Ʈ�� ��ũ���� ������ (0, 0)�� ������ ��ũ�� ��ǥ�踦 ����Ѵ�. ������ ������ 0.2f�� ������ �������� ���� ��ǥ�� �ȴ�.
	static Vector2 GetScreenPointByRatio(float x, float y);
};