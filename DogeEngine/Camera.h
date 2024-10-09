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
	// 카메라의 너비, 높이를 담은 RECT를 반환합니다.
	RectF GetScreenRect();
	// 카메라가 담는 영역의 좌측 하단 꼭지점의 월드좌표를 반환합니다.
	Vector2 GetCameraMinPosition();
	// 카메라가 담는 영역의 우측 상단 꼭지점의 월드좌표를 반환합니다.
	Vector2 GetCameraMaxPosition();

	// 여기다 따로 두는 건 좀 이상한데, 그냥 카메라 컴포넌트의 scale값을 없애고 BeglongedObject의 스케일값을 이용하는 게 나을 수도 있음
	void EasyMove_ShakeScale(float magnitude, float duration, int frequency = 0, bool startToUpscale = true, bool shrinkMagnitude = true, bool restore_transform_when_done = false);

	static Camera* GetMainCamera();

	// <08-06> View(클라이언트), Screen(중앙이 0, 0이고 좌우로 클라이언트 크기만큼), World 제각각임 // Screen 좌표는 그냥 좌상단 (0, 0) 클라이언트 좌표 쓰던지 좌표게 이름 명확하게 구분해서 변환하는 함수 각각 만들던지 하자
	static Vector2 ScreenToWorldPoint(Vector2 point);
	static Vector2 ScreenToWorldPoint(float x, float y);

	static Vector2 WorldToScreenPoint(Vector2 point);
	static Vector2 WorldToScreenPoint(float x, float y);

	// 화면의 좌측 하단을 (0%, 0%)로 삼는다 / UI 오브젝트의 위치를 지정할 때 사용. UI 오브젝트는 스크린의 중점이 (0, 0)인 개조된 스크린 좌표계를 사용한다. 예컨대 비율로 0.2f는 중점을 기준으로 음수 좌표가 된다.
	static Vector2 GetScreenPointByRatio(float x, float y);
};