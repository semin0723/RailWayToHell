#pragma once
#include "../DogeEngine/ActionNode.h"

class MonsterIdle : public ActionNode
{
public:
	MonsterIdle() {}
	virtual ~MonsterIdle() {}

	virtual void Initialize();
	virtual BTStatus Behave();
};

