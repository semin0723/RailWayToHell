#include "EventManager.h"
#include "EventHandler.h"
#include "Object.h"
#include "Scene.h"

void EventManager::AddEventListener(EventType type, std::function<void()> func)
{
	DG::EventHandler* eventHandler = GetOwner()->GetOwner()->GetEventHandler();

	if (type == EventType::ClickDown)
	{
		eventHandler->RegisterClickDownEventListener(GetOwner(), func);
	}
	else if (type == EventType::RightClickDown)
	{
		eventHandler->RegisterRightClickDownEventListener(GetOwner(), func);
	}
	else if (type == EventType::ClickUp)
	{
		eventHandler->RegisterClickUpEventListener(GetOwner(), func);
	}
	else if (type == EventType::RightClickUp)
	{
		eventHandler->RegisterRightClickUpEventListener(GetOwner(), func);
	}
}