#pragma once
#include <functional>
#include <unordered_map>
#include <list>
#include "defines.h"

class Object;

// ����: �Ϲ������� ���ٽ��� ȣ��� ������ ������ ĸ���� ������ ���� ������ ������ �����Ͽ� ����������, std::function���� ���ٽ��� ��üȭ�ϰ�, function�� ȣ���Ͽ� ���ٽ��� �����ϴ� ���, �����δ� function ��ü ���ο��� ���ٽ��� ���� �� �����ϴ� �Ͱ� �����Ƿ�, ���ٽ��� ������ ����� ���¸� �����ϰ� �ȴ�. function�� ���������� �ȿ� ���ٽ��� �������� �����ϰ�, �� �������� function�� ȣ���� ������ �ʱ�ȭ���� �ʴ´ٴ� �Ҹ���.

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

    // Delay - delay���� ���� / Duration - duration���� ����
    // duration ����� ��� countPara�� ���õ� (loop�� ����?)
    // loop�� false�� ���, ������ Update���� done �÷��׷� true���� ���޵ȴ�.
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