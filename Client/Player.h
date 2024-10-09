#pragma once
#include "Engine.h"
#include "PlayerEnum.h"
#include "GameObject.h"
#include "defines.h"
#include "HealthComponent.h"
#include "Config.h"
#include "GameEffect.h"

struct OnPlayerDie : public IOnDie
{
	void operator()() override;

	class Player* player;
};

struct OnPlayerHit : public IOnHit
{
	void operator()(int damage) override;

	class Player* player;
};

class Player : public GameObject
{
public:
	bool isTutorial = true;
	bool isTutorialBT = true;
	int tutorial_turn = 1;

	class GameManager* gm;
	class WeaponStateUI* currentWeaponTypeUI;

	BT::BehaviourTreeV2* playerState;

	SpriteRenderer* renderer;
	Animator* animator;

	AttackType attackType{};
	GameObject* attackTarget{ nullptr };
	PlayerPhase phase{ PlayerPhase::Idle };

	MoveType moveType;
	WeaponType currentWeapon{ WeaponType::Gun };
	OnPlayerDie onPlayerDie;
	OnPlayerHit onPlayerHit;

	GameEffect* gameEffect = nullptr;

	void Awake() override;
	void Start() override;
	void Update() override;
	void OnDestroy() override;

	void ReStart();
	void StateChanged();
	bool CanMove(int index);

	bool IsAimPhase();
	void SetAttackTarget(GameObject* target);
	//void SetKnockbackTarget(GameObject* target);

	void SwapWeaponType();


	void UpdateSprite(std::string key);


	bool init_attack = false;
	// 공격 대상 인덱스가 아니라 이동할 타일 인덱스임
	int moveTargetTileIndex{ 0 };
	float attack_elapsedTime = 0.0f;
	int attack_step = 0;
	int attackDir = 1;
	bool Attack();
	bool KnockbackAttack();

	void Shot();
	void TutorialEnd();
};
//
//class Player_TutorialMove : public State
//{
//	Player* player;
//	bool action_fixed = false;
//	float initX = 0.0f;
//	float targetX = 0.0f;
//	float t = 0.0f;
//	bool init = false;
//
//
//	float t2_block_player_action_time = 1.0f;
//	float t2_block_player_action_elapsed_time = 0.0f;
//
//
//	float t3_block_player_action_time = 1.0f;
//	float t3_block_player_action_elapsed_time = 0.0f;
//
//public:
//	void Initialize() override;
//	void OnStateEnter() override;
//	void Update() override;
//	void OnStateExit() override;
//
//	void Tutorial_Init();
//	void Tutorial_Enter();
//	void Tutorial_Exit();
//
//	bool Tutorial_Action();
//	bool turn1_init = false;
//	bool Tutorial_Turn1();
//	bool turn2_init = false;
//	bool Tutorial_Turn2();
//	bool turn3_init = false;
//	bool turn3_init2 = false;
//	bool Tutorial_Turn3();
//};
//
//
//
//
//class Player_TutorialAim : public State
//{
//	Player* player;
//
//
//	float t4_dialog1ShowingInterval = 1.5f;
//	float t4_dialog1ShowedDuration = 2.0f;
//	float t4_dialog1ShowingDuration = 0.4f;
//public:
//	void Initialize() override;
//	void OnStateEnter() override;
//	void Update() override;
//	void OnStateExit() override;
//
//	void Shot();
//
//	void Tutorial_Init();
//	void Tutorial_Enter();
//	void Tutorial_Exit();
//
//	bool Tutorial_Action();
//	bool turn1_init = false;
//	bool Tutorial_Turn1();
//	bool turn2_init = false;
//	bool Tutorial_Turn2();
//	bool turn3_init = false;
//	bool turn3_press_init = false;
//	bool turn3_dialog_init = false;
//	bool turn3_dialog_init2 = false;
//	bool Tutorial_Turn3();
//};
//
//class Player_TutorialFinishMove : public State
//{
//	Player* player;
//	float t = 0.0f;
//public:
//	void Initialize() override;
//	void OnStateEnter() override;
//	void Update() override;
//	void OnStateExit() override;
//
//	bool Move();
//};
//
//class Player_TutorialAttack : public State
//{
//	Player* player;
//	bool init = false;
//	float flickering_elapsedTime = 0.0f;
//	int step = 0;
//
//	int targetIndex = 0;
//	int attackDir = 0;
//	int afterIndex = 0;
//public:
//	void Initialize() override;
//	void OnStateEnter() override;
//	void Update() override;
//	void OnStateExit() override;
//};