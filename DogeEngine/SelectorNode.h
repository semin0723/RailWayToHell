#pragma once
#include "BTStatus.h"
#include "BaseBTNode.h"

class SelectorNode : public BaseBTNode
{
public:
	SelectorNode(std::string name = "SelectorNode") { Name = name; }
	virtual ~SelectorNode() {}

	void AttachChild(BaseBTNode* node);
	void AttachChildAt(BaseBTNode* node, unsigned int index);
	bool DetachChild(std::string nodeName);
	void DetachChildAt(unsigned int index);

protected:
	std::vector<BaseBTNode*> Childs;
};

