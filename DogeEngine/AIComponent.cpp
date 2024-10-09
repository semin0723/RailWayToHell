#include "AIComponent.h"
#include "BehaviorTree.h"
#include "BaseBTNode.h"

void AIComponent::Awake()
{
	root = new BehaviorTree;
}

void AIComponent::Start()
{

}

void AIComponent::Update()
{
	
	root->Update();
}

void AIComponent::OnDestroy() {
	delete root;
}

void AIComponent::SetRootNode(BaseBTNode* node)
{
	root->SetRootNode(node);
}
