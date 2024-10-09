#pragma once
#include "../DogeEngine/ActionNode.h"

struct FunctionCommand;

class MonsterRush : public ActionNode
{
public:
	MonsterRush() {}
	virtual ~MonsterRush();

	virtual void Initialize();
	virtual BTStatus Behave();

	void DetectObject();

	bool isFinish;
	bool isRush;

	FunctionCommand* action;
	bool isRegistAction = false;
};

