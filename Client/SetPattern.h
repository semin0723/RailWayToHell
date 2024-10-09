#pragma once
#include "Engine.h"

class SetPattern : public RandomSelectorNode
{
public:
	SetPattern() {}
	virtual ~SetPattern() {}

	virtual void Initialize();
	virtual BTStatus Behave();

private:
	int actionCount = 0;
	int curBossHp = 3;
};

