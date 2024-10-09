#include "TimerFunctionManager.h"
#include "Object.h"
#include "Time.h"

TimerFunction::TimerFunction(std::function<void(bool)> func, TimerFunctionType type, TimerFunctionTimeType timeType, float timePara, int countPara)
{
    this->func = func;
    this->type = type;
    this->timeType = timeType;

    if (type == TimerFunctionType::Delay)
    {
        delay = timePara;
        repeatCount = countPara;
        if (repeatCount <= 0) KillTimerFunction();
    }
    else if (type == TimerFunctionType::Duration)
    {
        duration = timePara;
        repeatCount = countPara;
        if (repeatCount <= 0) KillTimerFunction();
    }
}

void TimerFunction::Update()
{
    if (enable == false) return;

    if (type == TimerFunctionType::Delay)
    {
        if (timeType == TimerFunctionTimeType::MultiedTime)
        {
            elapsedTime += Time::GetDeltaTime();
        }
        else if (timeType == TimerFunctionTimeType::RealTime)
        {
            elapsedTime += Time::GetRealDeltaTime();
        }
        
        if (elapsedTime >= delay)
        {
            elapsedTime -= delay;
            repeatCount--;
            if (repeatCount > 0)
            {
                func(false);
            }
            else
            {
                func(true);
                KillTimerFunction();
            }
        }
    }
    else if (type == TimerFunctionType::Duration)
    {
        if (timeType == TimerFunctionTimeType::MultiedTime)
        {
            elapsedTime += Time::GetDeltaTime();
        }
        else if (timeType == TimerFunctionTimeType::RealTime)
        {
            elapsedTime += Time::GetRealDeltaTime();
        }

        if (elapsedTime < duration)
        {
            func(false);
        }
        else
        {
            func(true);
            repeatCount--;
            if (repeatCount > 0)
            {
                elapsedTime -= duration;
            }
            else
            {
                KillTimerFunction();
            }
        }
    }
}

bool TimerFunction::GetDestroyState()
{
    return destroy;
}

void TimerFunction::KillTimerFunction()
{
    destroy = true;
    enable = false;
}








NAMESPACE_DOGEENGINE_BEGIN

TimerFunction* TimerFunctionManager::RegisterRealTimeTimerFunction(Object* subject, std::function<void(bool)> func, bool duration_mode, float time_para, int count_para)
{
    TimerFunction* tf = nullptr;
    if (duration_mode == false)
    {
        tf = new TimerFunction(func, TimerFunctionType::Delay, TimerFunctionTimeType::RealTime, time_para, count_para);
    }
    else if (duration_mode == true)
    {
        tf = new TimerFunction(func, TimerFunctionType::Duration, TimerFunctionTimeType::RealTime, time_para, count_para);
    }
    timerFunctions[subject].push_back(tf);
    return tf;
}

TimerFunction* TimerFunctionManager::RegisterTimerFunction(Object* subject, std::function<void(bool)> func, bool duration_mode, float time_para, int count_para)
{
    TimerFunction* tf = nullptr;
    if (duration_mode == false)
    {
        tf = new TimerFunction(func, TimerFunctionType::Delay, TimerFunctionTimeType::MultiedTime, time_para, count_para);
    }
    else if (duration_mode == true)
    {
        tf = new TimerFunction(func, TimerFunctionType::Duration, TimerFunctionTimeType::MultiedTime, time_para, count_para);
    }
    timerFunctions[subject].push_back(tf);
    return tf;
}
void TimerFunctionManager::DeleteTimerFunction(Object* subject)
{
    if (timerFunctions.find(subject) == timerFunctions.end()) return;

    std::list<TimerFunction*>& tfs = timerFunctions[subject];
    for (TimerFunction* tf : tfs)
    {
        delete tf;
    }
    timerFunctions.erase(subject);
}
void TimerFunctionManager::Update()
{
    if (enable == false) return;

    // p를 참조가 아닌 복사로 받을 경우 p.second.erase(iter)로 지워지는 건 복사된 리스트의 요소일 뿐이므로, 원본 리스트에는 그대로 남아있는 TimerFunction 객체를 delete하는 결과를 가져오므로 주의하도록 한다.
    for (auto& p : timerFunctions)
    {
        if (p.first->GetEnable() == false || p.first->GetStarted() == false) continue;

        for (auto iter = p.second.begin(); iter != p.second.end();)
        {
            if ((*iter)->GetDestroyState() == true)
            {
                TimerFunction* tf = (*iter);
                iter = p.second.erase(iter);
                delete tf;
            }
            else
            {
                (*iter)->Update();
                ++iter;
            }
        }
    }
}
void TimerFunctionManager::PauseUpdate()
{
    enable = false;
}
void TimerFunctionManager::ResumeUpdate()
{
    enable = true;
}
NAMESPACE_DOGEENGINE_END