#pragma once
#include "Engine.h"

class PositionHelper : public Object
{
	bool move = false;
	int currentTargetIdx = 0;
	Object* currentTarget = nullptr;
public:
	std::vector<Object*> targets;


	void AddTarget(Object* target)
	{
		targets.push_back(target);
		if (targets.size() == 1) currentTarget = target;
	}

	void Update()
	{
		if (Input::IsDown(KeyCode::Space))
		{
			printf("change target... %f, %f\n", currentTarget->transform->GetPosition().x, currentTarget->transform->GetPosition().y);
			currentTargetIdx++;
			if (targets.size() <= currentTargetIdx)
			{
				currentTargetIdx = 0;
			}
			currentTarget = targets[currentTargetIdx];
		}

		if (Input::IsPress(KeyCode::A))
		{
			currentTarget->transform->Translate(-1, 0, 0);
			move = true;
		}
		else if (Input::IsPress(KeyCode::D))
		{
			currentTarget->transform->Translate(1, 0, 0);
			move = true;
		}
		if (Input::IsPress(KeyCode::W))
		{
			currentTarget->transform->Translate(0, 1, 0);
			move = true;
		}
		else if (Input::IsPress(KeyCode::S))
		{
			currentTarget->transform->Translate(0, -1, 0);
			move = true;
		}

		if (move)
		{
			printf("current position: %f, %f\n", currentTarget->transform->GetPosition().x, currentTarget->transform->GetPosition().y);
			move = false;
		}
	}
};