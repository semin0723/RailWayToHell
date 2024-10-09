#pragma once
#include "Engine.h"
#include "ICommand.h"

class AttackTile : public ActionNode
{
public:
	AttackTile() {
		attackTile = {
			{0, 6},
			{1, 5},
			{2, 4}
		};
	}
	~AttackTile() {}

	virtual void Initialize();
	virtual BTStatus Behave();

private:
	FunctionCommand* action = nullptr;

	int waitTurn = 0;
	bool isWait = true;

	bool isRegistAction = false;
	bool leftSpawn = true;
	bool rightSpawn = true;

	std::vector<std::pair<int, int>> attackTile;

	std::pair<int, int> GetAttackArea();
};

