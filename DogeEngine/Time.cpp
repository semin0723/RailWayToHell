#include "Time.h"

Time::Time()
{
	InitTime();
}
Time::~Time()
{

}
Time& Time::GetInstance()
{
	static Time instance;
	return instance;
}
void Time::InitTime()
{
	//preTime = curTime = clock();
	QueryPerformanceFrequency(&frequency);

	QueryPerformanceCounter(&curCount);
	QueryPerformanceCounter(&preCount);
	deltaTime = 0;

	//fixedPreTime = fixedCurTime = clock();
	QueryPerformanceCounter(&curCountFixed);
	QueryPerformanceCounter(&preCountFixed);
	fixedDeltaTime = 0;

	updateCountPerSecond = 0;
	fixedUpdateCountPerSecond = 0;

	updateCount = 0;
	fixedUpdateCount = 0;
}
void Time::UpdateTime()
{
	//preTime = curTime;
	//curTime = clock();
	//deltaTime = curTime - preTime;
	preCount = curCount;
	QueryPerformanceCounter(&curCount);
	// float 변환 안 해주면 소수점 자리 절삭되면서 실제 deltaTime보다 작은 값이 담기니 주의
	deltaTime = (float)(curCount.QuadPart - preCount.QuadPart) / (float)(frequency.QuadPart);

	static float elapsedTime = 0;
	elapsedTime += deltaTime;
	if (elapsedTime >= 1.0f)
	{
		elapsedTime -= 1.0f;
		updateCountPerSecond = updateCount;
		updateCount = 0;
		fixedUpdateCountPerSecond = fixedUpdateCount;
		fixedUpdateCount = 0;
	}
}
void Time::UpdateFixedTime()
{
	//fixedPreTime = fixedCurTime;
	//fixedCurTime = clock();
	//fixedDeltaTime = fixedCurTime - fixedPreTime;
	preCountFixed = curCountFixed;
	QueryPerformanceCounter(&curCountFixed);
	fixedDeltaTime = (float)(curCountFixed.QuadPart - preCountFixed.QuadPart) / (float)(frequency.QuadPart);
}


void Time::SetDeltaTime(float _deltaTime)
{
	deltaTime = _deltaTime;
}

float Time::GetRealDeltaTime()
{
	if (GetInstance().getTimeReturnZero == true)
	{
		return 0.0f;
	}
	return GetInstance().deltaTime;
}

float Time::GetRealFixedDeltaTime()
{
	if (GetInstance().getTimeReturnZero == true)
	{
		return 0.0f;
	}
	return GetInstance().fixedDeltaTime;
}

float Time::GetDeltaTime()
{
	return GetInstance()._GetDeltaTime();
}
float Time::_GetDeltaTime()
{
	if (getTimeReturnZero == true)
	{
		return 0.0f;
	}
	return deltaTime * speedMult;
}
float Time::GetFixedDeltaTime()
{
	return GetInstance()._GetFixedDeltaTime();
}
void Time::SetSpeedMult(float mult)
{
	GetInstance().speedMult = mult;
}
float Time::_GetFixedDeltaTime()
{
	if (getTimeReturnZero == true)
	{
		return 0.0f;
	}
	return fixedDeltaTime * speedMult;
}


float Time::GetFrameRate()
{
	return 1.0f / deltaTime;
}
long long Time::GetUpdateCountPerSecond()
{
	return updateCountPerSecond;
}
long long Time::GetFixedUpdateCountPerSecond()
{
	return fixedUpdateCountPerSecond;
}