#pragma once
#include "Engine.h"
#include "ICommand.h"

class OrderStaticTileAttack : public ActionNode
{
public:
	OrderStaticTileAttack() {
		attackTile = {
		{0, 6},
		{1, 5},
		{2, 4}
		};
	}
	~OrderStaticTileAttack();

	virtual void Initialize();
	virtual BTStatus Behave();

private:
	FunctionCommand* action = nullptr;

	std::vector<std::pair<int, int>> attackTile;

	std::pair<int, int> GetAttackArea();
};

