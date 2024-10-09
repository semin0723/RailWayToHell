#pragma once
#include "Engine.h"

class BossAIComponent : public AIComponent
{
public:
	BossAIComponent() {}
	~BossAIComponent() {}

	virtual void Start();
	virtual void Update();
};

