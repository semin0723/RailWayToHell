#include "BehaviourTreeV2.h"
#include "cassert"

BEGINBT

BehaviourTreeV2::~BehaviourTreeV2()
{
	if (rootNode)
	{
		delete rootNode;
	}
}

void BehaviourTreeV2::Behave(float deltaTime)
{
	if (state != Status::Running)
	{
		rootNode->OnExit();
		rootNode->OnEnter();
	}
	state = rootNode->Behave(deltaTime);
}

Status::Type Node::Behave(float deltaTime)
{
	Node* parentNode = GetParentNode();
	if (parentNode)
	{
		blackBoard = parentNode->blackBoard;
	}
	return OnBehave(deltaTime);
}
BehaviourTreeV2* Node::GetBehaviourTree()
{
	ReflectObject* outer = GetOuter();
	while (outer)
	{
		if (BehaviourTreeV2* bt = dynamic_cast<BehaviourTreeV2*>(outer); bt) return bt;
		outer = outer->GetOuter();
	}
	assert(0);
	return nullptr;
}

Node::~Node()
{
	OnExit();
}


ControlNode::~ControlNode()
{
	for (auto& item : childNodes)
	{
		delete item;
	}
}
void ControlNode::OnEnter()
{
	currentNode = childNodes.begin();
	(*currentNode)->OnEnter();

}

void ControlNode::OnExit()
{
	if (currentNode._Ptr && currentNode != childNodes.end())
	{
		(*currentNode)->OnExit();
	}
}

void ControlNode::NextNode()
{
	(*currentNode)->OnExit();

	++currentNode;
	if (currentNode != childNodes.end())
	{
		(*currentNode)->OnEnter();
	}
}

Status::Type SequenceNode::OnBehave(float deltaTime)
{
	Status::Type stat = GetCurrentNode()->Behave(deltaTime); 

	if (stat == Status::Success)
	{
		NextNode();
		if (currentNode != childNodes.end())
		{
			return OnBehave(deltaTime);
		}
		else
		{
			return Status::Success;
		}
	}

	return stat;
}

Status::Type SelectorNode::OnBehave(float deltaTime)
{
	Status::Type stat = GetCurrentNode()->Behave(deltaTime);

	if (stat == Status::Failure)
	{
		if (++currentNode != childNodes.end())
		{
			return OnBehave(deltaTime);
		}
		else
		{
			return Status::Failure;
		}
	}

	return stat;
}

Status::Type BlackBoardDecorator::OnBehave(float deltaTime)
{
	blackboardOrigin.SetParent(blackBoard);

	blackBoard = &blackboardOrigin;
	return Decorator::OnBehave(deltaTime);
}


Decorator::~Decorator()
{
	delete nextNode;
}

ENDBT
