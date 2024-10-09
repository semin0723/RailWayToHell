#pragma once
#include <string>
#include "BaseBTNode.h"

class BehaviorTree
{
public:
	BehaviorTree() {}
	~BehaviorTree() { 
		delete Root; 
		Root = nullptr;
	}

	void Update();

	template <class T>
	BaseBTNode* CreateNode(std::string name) {
		BaseBTNode* newNode = new T(name);

		return newNode;
	}

	void SetRootNode(BaseBTNode* node) { Root = node; }


private:
	BaseBTNode* Root = nullptr;
};

