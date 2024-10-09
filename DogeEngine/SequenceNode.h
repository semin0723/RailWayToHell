#pragma once
#include "BTStatus.h"
#include "BaseBTNode.h"

class SequenceNode : public BaseBTNode
{
public:
	SequenceNode(std::string name = "SequenceNode") { Name = name; }
	virtual ~SequenceNode() {}

	void AttachChild(BaseBTNode* node);
	void AttachChildAt(BaseBTNode* node, unsigned int index);
	bool DetachChild(std::string nodeName);
	void DetachChildAt(unsigned int index);

protected:
	std::vector<BaseBTNode*> Childs;
};

