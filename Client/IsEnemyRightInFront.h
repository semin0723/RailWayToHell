#pragma once
#include "../DogeEngine/DecoratorNode.h"

class IsEnemyRightInFront : public DecoratorNode
{
public:
	IsEnemyRightInFront(std::string name = "DecoratorNode") { Name = name; }
	virtual ~IsEnemyRightInFront();

	virtual void Initialize();
	virtual BTStatus Behave();
};

