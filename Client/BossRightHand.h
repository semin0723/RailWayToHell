#pragma once
#include "Engine.h"
#include "GameObject.h"
#include "ICommand.h"

class Boss;

class BossRightHand : public GameObject
{
public:
	BossRightHand() {}
	~BossRightHand() {}

	virtual void Awake() override;

	virtual void OnDestroy() override;

	Boss* body;

	FunctionCommand* actionOwner = nullptr;

	bool created = true;
private:
	SpriteRenderer* spriteRenderer = nullptr;
};

