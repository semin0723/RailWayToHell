#pragma once
#include "BaseBTNode.h"
#include "BtStatus.h"

class RandomSelectorNode :public BaseBTNode
{
public:
	RandomSelectorNode(std::string name = "RandomSelectorNode") { Name = name; }
	virtual ~RandomSelectorNode() {}

	bool SetChildNodeProbability(unsigned int idx, float probability);
	void AttachChild(BaseBTNode* node);
	void AttachChildAt(BaseBTNode* node, unsigned int index);
	bool DetachChild(std::string nodeName);
	void DetachChildAt(unsigned int index);

protected:
	std::vector<float> ChildNodeProbaility;
	std::vector<BaseBTNode*> Childs;
};

