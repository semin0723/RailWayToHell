#pragma once
#include "../DogeEngine/DecoratorNode.h"

class CheckPhase : public DecoratorNode
{
public:
	CheckPhase() {}
	virtual ~CheckPhase();

	virtual void Initialize();
	virtual BTStatus Behave();

};

