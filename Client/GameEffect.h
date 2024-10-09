#pragma once
#include "Engine.h"

class GameObject;

// VFX�� ī�޶�� ���� �������� �ϹǷ� UI�� �߰��ϸ� �� ��
class GameEffect : public Object
{
public:
	GameEffect() {}
	~GameEffect() {}

	virtual void Awake() override;
	virtual void Update() override;

	void CreateEffect(std::string key, Vector3 position, GameObject* target = nullptr);
private:
	SpriteRenderer* spriteRenderer = nullptr;
	Animator* animator = nullptr;

	GameObject* targetObject = nullptr;

	Animation crossbowFlash;
	Animation muzzleFlash;
	Animation spawnEffect;
	Animation spawnWaitEffect;
	Animation trainSpark;
	Animation chanDrop;
	Animation explosion;

	Animation acidFire;
	Animation claw;
	Animation bite;
};

