#include <cmath>
#include <algorithm>
#include "Math.h"
#include "Time.h"


float PI = 3.14159265f;

float Math::Lerp(float start, float end, float t)
{
	return start * (1-t) + end * t;
}

float Math::EaseInCubic(float start, float end, float t)
{
	end -= start;
	return end * t * t * t + start;
}
Vector3 Math::EaseInCubic(Vector3 start, Vector3 end, float t)
{
	end -= start;
	return end * t * t * t + start;
}

float Math::EaseOutCubic(float start, float end, float t)
{
	t -= 1.0f;
	end -= start;
	return end * (t * t * t + 1) + start;
}
Vector3 Math::EaseOutCubic(Vector3 start, Vector3 end, float t)
{
	t -= 1.0f;
	end -= start;
	return end * (t * t * t + 1) + start;
}

float Math::EaseInOutCubic(float start, float end, float t)
{
	t /= 0.5f;
	end -= start;
	if (t < 1.f)
	{
		return end / 2.f * t * t * t + start;
	}
	t -= 2.0f;
	return end / 2.f * (t * t * t + 2) + start;
}

float Math::Deg2Rad(float degree)
{
	return degree * PI / 180;
}

float Math::Rad2Deg(float radian)
{
	return radian * 180 / PI;
}

float Math::SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed)
{
	float deltaTime = Time::GetDeltaTime();

	smoothTime = std::max<float>(0.0001f, smoothTime);
	float omega = 2.f / smoothTime;

	float x = omega * deltaTime;
	float exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
	float change = current - target;
	float originalTo = target;

	float maxChange = maxSpeed * smoothTime;
	change = Clamp<float>(change, -maxChange, maxChange);
	target = current - change;

	float temp = (currentVelocity + omega * change) * deltaTime;
	currentVelocity = (currentVelocity - omega * temp) * exp;
	float output = target + (change + temp) * exp;

	if (originalTo - current > 0.0f == output > originalTo)
	{
		output = originalTo;
		currentVelocity = (output - originalTo) / deltaTime;
	}

	return output;
}

bool Math::Approximate(float a, float b, float epsilon)
{
	return abs(a - b) < epsilon;
}

bool Math::Approximate(Vector3& vec, float a, float epsilon)
{
	if (abs(vec.x - a) >= epsilon) return false;
	if (abs(vec.y - a) >= epsilon) return false;
	if (abs(vec.z - a) >= epsilon) return false;
	return true;
}

bool Math::Approximate(Vector2& vec, float a, float epsilon)
{
	if (abs(vec.x - a) >= epsilon) return false;
	if (abs(vec.y - a) >= epsilon) return false;
	return true;
}
