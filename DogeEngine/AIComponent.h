#pragma once
#include "Script.h"

class BaseBTNode;
class BehaviorTree;

class AIComponent : public Script
{
public:
	AIComponent() {}
	~AIComponent() {}

	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void OnDestroy();

protected:
	void SetRootNode(BaseBTNode* node);

private:
	BehaviorTree* root = nullptr;
};

