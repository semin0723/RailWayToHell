#pragma once
#include "Engine.h"
#include "GameObject.h"
#include "GameManager.h"
#include "ICommand.h"
#include "GameEffect.h"

enum class BossAnimationState { Default, Idle, Spawn, EyeOpen, EyeClose };

class Boss : public GameObject
{
public:
	Boss() {}
	~Boss() {}

	virtual void Awake() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	void EnableSelectable();
	void OpenEye();
	void CloseEye();

	void CreateHand();
	void IncreaseOpenWaitTurn() { waitTurn++; }
	bool IsEyeOpen() const { return isOpenEye; }

	BossAnimationState GetAnimationState() const { return animationState; }

	GameObject* targetPlayer = nullptr;
	GameObject* leftHand = nullptr;
	GameObject* rightHand = nullptr;

	template<class T>
	void HandCrashed() {}

	GameObject* GetHand(int index);

	FunctionCommand* leftActionOwner = nullptr;
	FunctionCommand* rightActionOwner = nullptr;
	bool IsHandAlive();


	bool isBossStageReady = false;
	int waitTurn = 0;
private:
	SpriteRenderer* spriteRenderer = nullptr;
	Animator* animator = nullptr;

	bool isOpenEye = true;
	bool isHalfClose = false;
	float animationChangeInterval = 0.5f;

	BossAnimationState animationState = BossAnimationState::Default;

	Animation anim_bossSpawn;
	Animation anim_bossEyeOpen;
	Animation anim_bossEyeClose;

	Animation anim_bossEyeCloseHalf1;
	Animation anim_bossEyeCloseHalf2;

	std::vector<std::pair<int, int>> spawnLocation;
	std::pair<int, int> curSpawnTileIndex;

	GameEffect* spawnEffectLeft = nullptr;
	GameEffect* spawnEffectRight = nullptr;

	Object* lock = nullptr;
};

template <>
void Boss::HandCrashed<BossLeftHand>();

template <>
void Boss::HandCrashed<BossRightHand>();