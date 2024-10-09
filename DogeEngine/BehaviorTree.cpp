#include "BehaviorTree.h"

void BehaviorTree::Update()
{
	if (Root->GetNodeState() != BTStatus::Running) {
		Root->Initialize();
	}
	Root->Behave();
}
