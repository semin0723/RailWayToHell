#include "Script.h"
#include "Object.h"
#include "TimerFunctionManager.h"


void Script::SetObjectEnable(bool enable)
{
	GetOwner()->SetEnable(enable);
}
void Script::SetDontDestroyOnSceneChange(bool dontDestroy, bool initializeOnSceneChange)
{
	GetOwner()->SetDontDestroyOnSceneChange(dontDestroy, initializeOnSceneChange);
}
void Script::AddTag(const char* _tag)
{
	GetOwner()->AddTag(_tag);
}
void Script::RemoveTag(const char* _tag)
{
	GetOwner()->RemoveTag(_tag);
}
bool Script::CompareTag(const char* _tag)
{
	return GetOwner()->CompareTag(_tag);
}
TimerFunction* Script::AddTimerFunction(std::function<void(bool)> func, bool duration_mode, float time_para, int count_para)
{
	return GetOwner()->AddTimerFunction(func, duration_mode, time_para, count_para);
}
void Script::Kill()
{
	GetOwner()->Kill();
}