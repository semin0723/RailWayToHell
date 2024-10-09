#pragma once
#include "Engine.h"
#include "../DogeEngine/SelectorNode.h"

class HasTargetInRange : public SelectorNode
{
public:
	HasTargetInRange(std::string name = "SelectorNode") { Name = name; }
	virtual ~HasTargetInRange();

	virtual void Initialize();
	virtual BTStatus Behave();
};

