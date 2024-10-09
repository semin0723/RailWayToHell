#pragma once
#include "Engine.h"

class FarMonsterAIComponent : public AIComponent
{
public:
	FarMonsterAIComponent() {}
	~FarMonsterAIComponent() {}

	virtual void Start();
	virtual void Update();
};

