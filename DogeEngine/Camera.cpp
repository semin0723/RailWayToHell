#include "Camera.h"
#include "Core.h"
#include "Object.h"
#include "Transform.h"
#include "Math.h"
#include "Time.h"

Camera::Camera()
{
	ChangeRect(DG::Core::GetInstance().GetScreenRect());
}
Transform* Camera::GetTransform()
{
	return GetOwner()->transform;
}
void Camera::ChangeRect(RectF clientRect)
{
	rect = clientRect;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	rectRatio = (float)width / (float)height;
}

void Camera::ChangeRect(float width, float height)
{
	this->width = width;
	this->height = height;
	rect = RectF{ 0, 0, width, height };
	rectRatio = width / height;
}

RectF Camera::GetScreenRect()
{
	return rect;
}

Vector2 Camera::GetCameraMinPosition()
{
	return ScreenToWorldPoint(0, height);
}
Vector2 Camera::GetCameraMaxPosition()
{
	return ScreenToWorldPoint(width, 0);
}

void Camera::EasyMove_ShakeScale(float magnitude, float duration, int frequency, bool startToUpscale, bool shrinkMagnitude, bool restore_transform_when_done)
{
	Object* obj = GetOwner();

	float initScale;
	if (restore_transform_when_done == true) initScale = this->scale;


	const float init_magnitude = magnitude;
	float radian = 0;

	if (frequency <= 0)
	{
		frequency = duration / 0.05f;
	}

	const float radPerSecond = Math::PI * frequency;
	const float maxRad = radPerSecond * duration;

	float dir;
	if (startToUpscale) dir = 1.f;
	else dir = -1.f;

	obj->AddTimerFunction([=](bool lastUpdate) mutable
		{
			if (lastUpdate == true && restore_transform_when_done == true)
			{
				this->scale = initScale;
			}
			else
			{
				if (shrinkMagnitude == true)
				{
					magnitude = init_magnitude * (maxRad - radian) / maxRad;
				}

				float dt = Time::GetDeltaTime();
				this->scale += std::cos(radian) * magnitude * dt * dir;
				radian += radPerSecond * dt;
			}
		}, true, duration, 1);
}

Camera* Camera::GetMainCamera()
{
	return DG::Core::GetInstance().GetCurrentScene()->mainCamera;
}

Vector2 Camera::ScreenToWorldPoint(Vector2 point)
{
	return ScreenToWorldPoint(point.x, point.y);
}
Vector2 Camera::ScreenToWorldPoint(float x, float y)
{
	Camera* mc = GetMainCamera();
	Vector3 camPos = mc->GetOwner()->transform->GetPosition();

	Vector2 worldPoint;
	worldPoint.x = (x * mc->scale) + (camPos.x - mc->width / 2 * mc->scale);
	worldPoint.y = ((mc->height - y) * mc->scale) + (camPos.y - mc->height / 2 * mc->scale); // y축 반전

	// 카메라 회전 ~ 회전행렬의 역행렬
	float rad = Math::Deg2Rad(mc->rotation);
	float oriX = worldPoint.x;
	float oriY = worldPoint.y;
	worldPoint.x = oriX * std::cos(rad) + oriY * std::sin(rad);
	worldPoint.y = -oriX * std::sin(rad) + oriY * std::cos(rad);


	return worldPoint;
}
Vector2 Camera::WorldToScreenPoint(Vector2 point)
{
	return WorldToScreenPoint(point.x, point.y);
}
Vector2 Camera::WorldToScreenPoint(float x, float y)
{
	Camera* mc = GetMainCamera();
	Vector3 camPos = mc->GetOwner()->transform->GetPosition();

	Vector2 screenPoint;
	screenPoint.x = (x - (camPos.x - mc->width * mc->scale / 2)) / mc->scale;
	screenPoint.y = -((y - mc->height) - (camPos.y - mc->height * mc->scale / 2)) / mc->scale; // y축 반전

	// 카메라 회전 ~ 반대는 회전행렬
	float rad = Math::Deg2Rad(mc->rotation);
	float oriX = screenPoint.x;
	float oriY = screenPoint.y;
	screenPoint.x = oriX * std::cos(rad) - oriY * std::sin(rad);
	screenPoint.y = oriX * std::sin(rad) + oriY * std::cos(rad);

	return screenPoint;
}
Vector2 Camera::GetScreenPointByRatio(float x, float y)
{
	Camera* mainCam = GetMainCamera();
	if (mainCam == nullptr) return Vector2{ 0,0 };
	return Vector2{ mainCam->width * x - mainCam->width / 2, mainCam->height * y - mainCam->height / 2 };
}