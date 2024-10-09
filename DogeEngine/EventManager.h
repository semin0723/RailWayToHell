#pragma once
#include <functional>
#include "Component.h"

enum class EventType { ClickDown, RightClickDown, ClickUp, RightClickUp };

class EventManager : public Component
{
public:
	void AddEventListener(EventType eventType, std::function<void()> func);
};
