#pragma once
#include "Engine.h"
#include "ICommand.h"
#include "GameManager.h"

class GameObject;

class AttackToPlayer : public ActionNode
{
public:
	AttackToPlayer() {}
	~AttackToPlayer() { Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->UnRegisterCommandList(action); }

	virtual void Initialize();
	virtual BTStatus Behave();

private:
	int waitTurn = 0;
	bool isWait = true;

	int attackObjectType = 0;

	bool isRegistAction = false;

	bool moveFinish = false;
	bool spawn = true;

	FunctionCommand* action = nullptr;

	int GetAttackSubject();
};

