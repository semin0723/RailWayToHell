#pragma once
#include "../DogeEngine/ActionNode.h"


struct FunctionCommand;

class MonsterAttack : public ActionNode
{
public:
	MonsterAttack() {}
	virtual ~MonsterAttack();

	virtual void Initialize();
	virtual BTStatus Behave();

	FunctionCommand* action;
	bool isRegistAction = false;
};

