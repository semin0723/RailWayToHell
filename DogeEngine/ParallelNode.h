#pragma once
#include "BTStatus.h"
#include "BaseBTNode.h"

class ParallelNode : public BaseBTNode
{
public:
	ParallelNode(std::string name = "ParallelNode") { Name = name; }
	virtual ~ParallelNode() {}

	void AttachChild(BaseBTNode* node);
	void AttachChildAt(BaseBTNode* node, unsigned int index);
	bool DetachChild(std::string nodeName);
	void DetachChildAt(unsigned int index);

protected:
	std::vector<BaseBTNode*> Childs;
};

