#include "EventHandler.h"
#include "Object.h"
#include "EventManager.h"
#include "Input.h"
#include "Camera.h"
#include "Collider.h"


NAMESPACE_DOGEENGINE_BEGIN

void EventHandler::RegisterClickDownEventListener(Object* eventTarget, std::function<void()> func)
{
	clickDownEventListener[eventTarget].push_back(func);
}
void EventHandler::DeleteClickDownEventListener(Object* eventTarget)
{
	clickDownEventListener.erase(eventTarget);
}

void EventHandler::RegisterClickUpEventListener(Object* eventTarget, std::function<void()> func)
{
	clickUpEventListener[eventTarget].push_back(func);
}
void EventHandler::DeleteClickUpEventListener(Object* eventTarget)
{
	clickUpEventListener.erase(eventTarget);
}

void EventHandler::RegisterRightClickDownEventListener(Object* eventTarget, std::function<void()> func)
{
	rightClickDownEventListener[eventTarget].push_back(func);
}
void EventHandler::DeleteRightClickDownEventListener(Object* eventTarget)
{
	rightClickDownEventListener.erase(eventTarget);
}

void EventHandler::RegisterRightClickUpEventListener(Object* eventTarget, std::function<void()> func)
{
	rightClickUpEventListener[eventTarget].push_back(func);
}
void EventHandler::DeleteRightClickUpEventListener(Object* eventTarget)
{
	rightClickUpEventListener.erase(eventTarget);
}

void EventHandler::DeleteAllEventListener(Object* eventTarget)
{
	DeleteClickDownEventListener(eventTarget);
	DeleteClickUpEventListener(eventTarget);
	DeleteRightClickDownEventListener(eventTarget);
	DeleteRightClickUpEventListener(eventTarget);
}



void EventHandler::Update()
{
	if (Input::IsDown(KeyCode::MouseLeft))
	{
		HandleClickEvent(clickDownEventListener);
	}
	else if (Input::IsUp(KeyCode::MouseLeft))
	{
		HandleClickEvent(clickUpEventListener);
	}
	if (Input::IsDown(KeyCode::MouseRight))
	{
		HandleClickEvent(rightClickDownEventListener);
	}
	else if (Input::IsUp(KeyCode::MouseRight))
	{
		HandleClickEvent(rightClickUpEventListener);
	}
}
void EventHandler::HandleClickEvent(std::unordered_map<Object*, std::vector<std::function<void()>>>& eventListener)
{
	Object* uppermostObject = nullptr;

	for (auto p : eventListener)
	{
		if (p.first->GetStarted() == false || p.first->GetEnable() == false) continue;
		if (p.first->GetComponent<EventManager>()->enable == false) continue;


		bool isCollided = false;
		std::vector<Collider*> colliders = p.first->GetComponents<Collider>();
		for (Collider* collider : colliders)
		{
			if (collider->IsCollideWithCursor())
			{
				isCollided = true;
				break;
			}
		}
		if (isCollided == false) continue;


		bool change = false;
		if (uppermostObject == nullptr)
		{
			change = true;
		}
		else if (uppermostObject->GetObjectLayer() == ObjectLayer::Object && p.first->GetObjectLayer() == ObjectLayer::UI)
		{
			change = true;
		}
		else if (uppermostObject->GetDrawingPriority() < p.first->GetDrawingPriority())
		{
			change = true;
		}

		if (change == true)
		{
			uppermostObject = p.first;
		}
	}

	if (uppermostObject != nullptr)
	{
		for (auto func : eventListener[uppermostObject])
		{
			func();
		}
	}
}

NAMESPACE_DOGEENGINE_END