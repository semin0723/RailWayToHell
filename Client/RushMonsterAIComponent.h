#pragma once
#include "Engine.h"

class RushMonsterAIComponent : public AIComponent
{
public:
	RushMonsterAIComponent() {}
	~RushMonsterAIComponent() {}

	virtual void Start();
	virtual void Update();
};

