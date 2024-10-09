#pragma once
#include "WinAPIFramework.h"

class Time
{
	// QPC
	LARGE_INTEGER frequency = { 0 };
	LARGE_INTEGER preCount = { 0 };
	LARGE_INTEGER curCount = { 0 };

	LARGE_INTEGER preCountFixed = { 0 };
	LARGE_INTEGER curCountFixed = { 0 };


	long long preTime;
	long long curTime;
	float deltaTime;

	long long fixedPreTime;
	long long fixedCurTime;
	float fixedDeltaTime;

	long long updateCountPerSecond;
	long long fixedUpdateCountPerSecond;

	Time();
	~Time();
	float _GetDeltaTime();
	float _GetFixedDeltaTime();
	float speedMult = 1.0f;
public:
	bool getTimeReturnZero = false;

	long long updateCount;
	long long fixedUpdateCount;

	static Time& GetInstance();
	void InitTime();

	void UpdateTime();
	void UpdateFixedTime();

	void SetDeltaTime(float _deltaTime);
	// speedMult를 곱하지 않고 실제 경과 시간을 반환
	static float GetRealDeltaTime();
	// speedMult를 곱하지 않고 실제 경과 시간을 반환
	static float GetRealFixedDeltaTime();
	// speedMult를 곱하여 반환 + 플래그에 의해 0을 반환할 수 있음
	static float GetDeltaTime();
	// speedMult를 곱하여 반환 + 플래그에 의해 0을 반환할 수 있음
	static float GetFixedDeltaTime();

	static void SetSpeedMult(float mult);

	float GetFrameRate();
	long long GetUpdateCountPerSecond();
	long long GetFixedUpdateCountPerSecond();
};

