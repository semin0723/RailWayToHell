#pragma once
#include "Engine.h"
#include "GameObject.h"
#include "defines.h"

class GameManager;

class RushTest : public GameObject
{
public:
	RushTest() {}
	~RushTest() {}

	GameManager* gm;
	Animation anim_idle;
	Animation anim_move;
	Animation anim_attack;
	Animator* animator;

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	bool isInitialize = false;

	virtual void SetAnimation(std::string key);
private:
	SpriteRenderer* spriteRenderer;
	AIComponent* aiComponent;
};

