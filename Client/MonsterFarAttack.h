#pragma once
#include "../DogeEngine/ActionNode.h"

struct FunctionCommand;

class MonsterFarAttack : public ActionNode
{
public:
	MonsterFarAttack() {}
	virtual ~MonsterFarAttack();

	virtual void Initialize();
	virtual BTStatus Behave();

	FunctionCommand* action;
	bool isRegistAction = false;
};

