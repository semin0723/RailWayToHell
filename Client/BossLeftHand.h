#pragma once
#include "Engine.h"
#include "GameObject.h"
#include "ICommand.h"

class Boss;

class BossLeftHand : public GameObject
{
public:
	BossLeftHand() {}
	~BossLeftHand() {}

	virtual void Awake() override;
	virtual void OnDestroy() override;

	Boss* body;
	FunctionCommand* actionOwner = nullptr;
	class MonsterComponent* monsterComponent{ nullptr };

	bool created = true;
private:
	SpriteRenderer* spriteRenderer = nullptr;
};

