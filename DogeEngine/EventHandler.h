#pragma once
#include <unordered_map>
#include <functional>
#include "defines.h"

class Object;

NAMESPACE_DOGEENGINE_BEGIN

class EventHandler
{
	// eventTarget, function
	std::unordered_map<Object*, std::vector<std::function<void()>>> clickDownEventListener;
	std::unordered_map<Object*, std::vector<std::function<void()>>> clickUpEventListener;
	std::unordered_map<Object*, std::vector<std::function<void()>>> rightClickDownEventListener;
	std::unordered_map<Object*, std::vector<std::function<void()>>> rightClickUpEventListener;
public:
	void RegisterClickDownEventListener(Object* eventTarget, std::function<void()> func);
	void DeleteClickDownEventListener(Object* eventTarget);

	void RegisterClickUpEventListener(Object* eventTarget, std::function<void()> func);
	void DeleteClickUpEventListener(Object* eventTarget);

	void RegisterRightClickDownEventListener(Object* eventTarget, std::function<void()> func);
	void DeleteRightClickDownEventListener(Object* eventTarget);

	void RegisterRightClickUpEventListener(Object* eventTarget, std::function<void()> func);
	void DeleteRightClickUpEventListener(Object* eventTarget);

	void DeleteAllEventListener(Object* eventTarget);

	void Update();
	void HandleClickEvent(std::unordered_map<Object*, std::vector<std::function<void()>>>& eventListener);
};

NAMESPACE_DOGEENGINE_END