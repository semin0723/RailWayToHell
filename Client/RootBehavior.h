#pragma once
#include "../DogeEngine/SelectorNode.h"

class RootBehavior : public SelectorNode
{
public:
	RootBehavior(std::string name = "SelectorNode") { Name = name; }
	virtual ~RootBehavior();

	virtual void Initialize();
	virtual BTStatus Behave();
};

