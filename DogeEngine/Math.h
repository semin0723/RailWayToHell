#pragma once
#include <xmmintrin.h>
#include "Vector2.h"
#include "Vector3.h"


class Math
{
public:
	static constexpr float PI = 3.14159265f;

	// 벡터의 Lerp는 Vector2, 3 내부로 옮김
	static float Lerp(float start, float end, float t);
	// 느리게 시작해서 점점 빨라짐
	static float EaseInCubic(float start, float end, float t);
	static Vector3 EaseInCubic(Vector3 start, Vector3 end, float t);
	// 빠르게 시작해서 점점 느려짐
	static float EaseOutCubic(float start, float end, float t);
	static Vector3 EaseOutCubic(Vector3 start, Vector3 end, float t);
	// 느리게 가다 빨라졌다 다시 느려짐
	static float EaseInOutCubic(float start, float end, float t);

	static float Deg2Rad(float degree);
	static float Rad2Deg(float radian);

	// 유니티의 SmoothDamp 로직
	// exp 구하는 부분은 지수함수를 테일러 급수로 3차항까지 전개한 근사치라고 함
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





	// 언리얼 엔진 역제곱근 빨리 구하기
	// __forceinline은 MSVC 예약어이며 inline보다 훨씬 강하게 inline화를 요구한다. 그럼에도 강제는 아니다.
	// inline 함수는 선언과 정의를 분리하지 않아야 한다.
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