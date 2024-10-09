#pragma once
#include "../DogeEngine/DecoratorNode.h"
class TargetIsNone : public DecoratorNode
{
public:
	TargetIsNone(std::string name = "DecoratorNode") { Name = name; }
	virtual ~TargetIsNone();

	virtual void Initialize();
	virtual BTStatus Behave();

};

