#pragma once
#include "../DogeEngine/DecoratorNode.h"

class TargetIsPlayer : public DecoratorNode
{
public:
	TargetIsPlayer(std::string name = "DecoratorNode") { Name = name; }
	virtual ~TargetIsPlayer();

	virtual void Initialize();
	virtual BTStatus Behave();

private:

};

