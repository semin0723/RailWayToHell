#pragma once
#include <xmmintrin.h>
#include "Vector2.h"
#include "Vector3.h"


class Math
{
public:
	static constexpr float PI = 3.14159265f;

	// ������ Lerp�� Vector2, 3 ���η� �ű�
	static float Lerp(float start, float end, float t);
	// ������ �����ؼ� ���� ������
	static float EaseInCubic(float start, float end, float t);
	static Vector3 EaseInCubic(Vector3 start, Vector3 end, float t);
	// ������ �����ؼ� ���� ������
	static float EaseOutCubic(float start, float end, float t);
	static Vector3 EaseOutCubic(Vector3 start, Vector3 end, float t);
	// ������ ���� �������� �ٽ� ������
	static float EaseInOutCubic(float start, float end, float t);

	static float Deg2Rad(float degree);
	static float Rad2Deg(float radian);

	// ����Ƽ�� SmoothDamp ����
	// exp ���ϴ� �κ��� �����Լ��� ���Ϸ� �޼��� 3���ױ��� ������ �ٻ�ġ��� ��
	static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed = 0xFFFFFFFF);


	static bool Approximate(float a, float b, float epsilon = 0.00001f);
	static bool Approximate(Vector3& vec, float a, float epsilon = 0.00001f);
	static bool Approximate(Vector2& vec, float a, float epsilon = 0.00001f);


	template <typename T>
	static T Clamp(T target, T min, T max)
	{
		if (target < min) target = min;
		else if (target > max) target = max;

		return target;
	}
	template <>
	static Vector3 Clamp(Vector3 target, Vector3 min, Vector3 max)
	{
		if (target.x < min.x) target.x = min.x;
		else if (target.x > max.x) target.x = max.x;

		if (target.y < min.y) target.y = min.y;
		else if (target.y > max.y) target.y = max.y;

		if (target.z < min.z) target.z = min.z;
		else if (target.z > max.z) target.z = max.z;

		return target;
	}
	template <>
	static Vector2 Clamp(Vector2 target, Vector2 min, Vector2 max)
	{
		if (target.x < min.x) target.x = min.x;
		else if (target.x > max.x) target.x = max.x;

		if (target.y < min.y) target.y = min.y;
		else if (target.y > max.y) target.y = max.y;

		return target;
	}





	// �𸮾� ���� �������� ���� ���ϱ�
	// __forceinline�� MSVC ������̸� inline���� �ξ� ���ϰ� inlineȭ�� �䱸�Ѵ�. �׷����� ������ �ƴϴ�.
	// inline �Լ��� ����� ���Ǹ� �и����� �ʾƾ� �Ѵ�.
	__forceinline static float GetSqrtFast(float In_float)
	{
		// Performs two passes of Newton-Raphson iteration on the hardware estimate
		//    v^-0.5 = x
		// => x^2 = v^-1
		// => 1/(x^2) = v
		// => F(x) = x^-2 - v
		//    F'(x) = -2x^-3

		//    x1 = x0 - F(x0)/F'(x0)
		// => x1 = x0 + 0.5 * (x0^-2 - Vec) * x0^3
		// => x1 = x0 + 0.5 * (x0 - Vec * x0^3)
		// => x1 = x0 + x0 * (0.5 - 0.5 * Vec * x0^2)
		//
		// This final form has one more operation than the legacy factorization (X1 = 0.5*X0*(3-(Y*X0)*X0)
		// but retains better accuracy (namely InvSqrt(1) = 1 exactly).

		const __m128 fOneHalf = _mm_set_ss(0.5f);
		__m128 Y0, X0, X1, X2, FOver2;
		float temp;

		Y0 = _mm_set_ss(In_float);
		X0 = _mm_rsqrt_ss(Y0);    // 1/sqrt estimate (12 bits)
		FOver2 = _mm_mul_ss(Y0, fOneHalf);

		// 1st Newton-Raphson iteration
		X1 = _mm_mul_ss(X0, X0);
		X1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X1));
		X1 = _mm_add_ss(X0, _mm_mul_ss(X0, X1));

		// 2nd Newton-Raphson iteration
		X2 = _mm_mul_ss(X1, X1);
		X2 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X2));
		X2 = _mm_add_ss(X1, _mm_mul_ss(X1, X2));

		_mm_store_ss(&temp, X2);
		return temp;
	}
};