#pragma once
#include "../DogeEngine/DecoratorNode.h"

class TargetIsMonster : public DecoratorNode
{
public:
	TargetIsMonster(std::string name = "DecoratorNode") { Name = name; }
	virtual ~TargetIsMonster();

	virtual void Initialize();
	virtual BTStatus Behave();

private:

};

