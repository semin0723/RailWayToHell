#pragma once
#include "../DogeEngine/ActionNode.h"

struct FunctionCommand;

class MonsterMove : public ActionNode
{
public:
	MonsterMove() {}
	virtual ~MonsterMove();

	virtual void Initialize();
	virtual BTStatus Behave();


	bool isMove;
	bool isFinish;

	FunctionCommand* action;
	bool isRegistAction = false;
};

