#pragma once
#include <string>
#include <vector>
#include <memory>
#include "BTStatus.h"
#include "AIComponent.h"

class AIComponent;

class BaseBTNode
{
public:
	BaseBTNode() = default;
	virtual ~BaseBTNode() = default;

	virtual void Initialize() = 0;
	virtual BTStatus Behave() = 0;

	const std::string& GetName() const { return Name; }
	void SetName(std::string& name) { Name = name; }
	const AIComponent* GetParent() const;
	void SetParent(AIComponent* parent);
	BTStatus GetCurStatus() const { return CurStatus; }

	BTStatus GetNodeState() const { return CurStatus; }

protected:
	std::string Name;
	BTStatus CurStatus = BTStatus::Default;
	AIComponent* Parent = nullptr;
};

