#pragma once
#include <functional>
#include <unordered_map>
#include <list>
#include "defines.h"

class Object;

// 주의: 일반적으로 람다식은 호출될 때마다 값으로 캡쳐한 변수를 원본 변수의 값으로 복사하여 가져오지만, std::function으로 람다식을 객체화하고, function을 호출하여 람다식이 동작하는 경우, 실제로는 function 객체 내부에서 람다식이 여러 번 동작하는 것과 같으므로, 람다식의 변수가 변경된 상태를 유지하게 된다. function의 스택프레임 안에 람다식의 변수들이 존재하고, 이 변수들이 function을 호출할 때마다 초기화되지 않는다는 소리다.

enum class TimerFunctionType { Delay, Duration };
enum class TimerFunctionTimeType { MultiedTime, RealTime };

class TimerFunction
{
private:
    bool destroy = false;
protected:
    std::function<void(bool)> func;
    TimerFunctionType type;
    TimerFunctionTimeType timeType{ TimerFunctionTimeType::MultiedTime };
    float elapsedTime = 0;

    // delay
    float delay = 0;
    int repeatCount = 0;

    // duration
    float duration = 0;
public:
    bool enable = true;

    // Delay - delay마다 실행 / Duration - duration동안 실행
    // duration 모드인 경우 countPara는 무시됨 (loop도 무시?)
    // loop가 false인 경우, 마지막 Update에선 done 플래그로 true값이 전달된다.
    TimerFunction(std::function<void(bool)> func, TimerFunctionType type, TimerFunctionTimeType timeType, float timePara = 0, int countPara = 1);

    void Update();
    bool GetDestroyState();
    void KillTimerFunction();
};



NAMESPACE_DOGEENGINE_BEGIN
class TimerFunctionManager
{
protected:
    std::unordered_map<Object*, std::list<TimerFunction*>> timerFunctions;
    bool enable = true;
public:
    TimerFunction* RegisterRealTimeTimerFunction(Object* subject, std::function<void(bool)> func, bool duration_mode, float time_para, int count_para);
    TimerFunction* RegisterTimerFunction(Object* subject, std::function<void(bool)> func, bool duration_mode, float time_para, int count_para);
    void DeleteTimerFunction(Object* subject);

    void Update();
    void PauseUpdate();
    void ResumeUpdate();

};
NAMESPACE_DOGEENGINE_END