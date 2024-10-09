#include "Player.h"
#include "GameManager.h"
#include "Cursor_Aim.h"
#include "Selectable.h"
#include "ICommand.h"
#include "HealthComponent.h"
#include "Highlightable.h"
#include "SceneEffect.h"
#include "Config.h"
#include "Trap.h"
#include "WeaponStateUI.h"
#include "ChatBalloonComponent.h"
#include "PlayerState.h"
#include "MonsterComponent.h"
#include "Boss.h"
#include "BossLeftHand.h"
#include "BossRightHand.h"



#define KeyDown(x) (Input::IsDown(KeyCode::x))

void OnPlayerDie::operator()()
{
	AudioFramework::Play_BankSound("Sound", "SFX_Player_Death");
	player->UpdateSprite("player/dead");
	player->renderer->SetSizeAuto();
	player->animator->enable = false;

	GameManager* gm = DG::Core::GetInstance().GetCurrentScene()->GetObjectByName<GameManager>("GameManager");
	gm->GameOver();
}

void OnPlayerHit::operator()(int damage)
{
	int hitSoundIdx = Random::Range(1, 3);
	AudioFramework::Play_BankSound("Sound", "SFX_Player_Hurt" + std::to_string(hitSoundIdx));
	player->UpdateSprite("player/hit");
	player->animator->enable = false;

	if (player->GetComponent<HealthComponent>()->GetHp() > damage) {
		GameEffect* hitEffect = player->AddObject<GameEffect>();
		int randomNum = Random::Range(1, 3);
		hitEffect->CreateEffect("VFX/HitBlood" + std::to_string(randomNum), player->transform->GetPosition() + Vector3(0, 100.f, 0));
		hitEffect->GetComponent<SpriteRenderer>()->SetSize(100.f, 100.f);
		hitEffect->SetDrawingPriority(OrderLayer::monster_hitEffect);
	}
	else {
		GameEffect* hitEffect = player->AddObject<GameEffect>();
		hitEffect->CreateEffect("VFX/DeadHit", player->transform->GetPosition() + Vector3(0, 100.f, 0));
		//hitEffect->GetComponent<SpriteRenderer>()->SetSize(100.f, 100.f);
		hitEffect->GetComponent<SpriteRenderer>()->SetSizeAuto();
		hitEffect->SetDrawingPriority(OrderLayer::monster_hitEffect);
	}

	player->GetComponent<HealthComponent>()->GetDamage(damage);

	player->AddTimerFunction([=](bool lu)
		{
			if(!player->GetComponent<HealthComponent>()->IsDie())
			{
				player->UpdateSprite("player/idle");
				player->animator->enable = true;
			}

		}, false, GetConfig().Player_HitSpriteDuration, 1);
}

struct PlayerAttackCommand : public ICommand
{
	virtual bool Do() override
	{
		return _this->Attack();
	}
	Player* _this;
};

struct PlayerKnockbackCommand : public ICommand
{
	virtual bool Do() override
	{
		return _this->KnockbackAttack();
	}
	Player* _this;
};


void Player::Awake()
{
	type = EType::Player;

	renderer = AddComponent<SpriteRenderer>();
	renderer->anchorType = AnchorType::BottomCenter;
	UpdateSprite("player/idle");
	renderer->SetSizeAuto();

	onPlayerDie.player = this;
	onPlayerHit.player = this;

	
	AddComponent<ChatBalloonComponent>();
	AddComponent<Highlightable_Player>();
	AddComponent<CircleCollider>();
	SetDrawingPriority(OrderLayer::Player);

	currentWeaponTypeUI = AddObject<WeaponStateUI>("CurrentWeaponType");

	auto anim_idle = ResourceManager::AddAnimation("player/idle");
	anim_idle.get()->AddGIF("player/idle");
	anim_idle.get()->loop = true;

	auto anim_Run = ResourceManager::AddAnimation("player/Run");
	anim_Run.get()->AddGIF("player/Run");
	anim_Run.get()->loop = true;


	auto mouse = ResourceManager::AddAnimation("UI/Mouse");
	mouse.get()->AddGIF("UI/Mouse");
	mouse.get()->loop = true;
	mouse->SetInterval(0.5f);

	animator = AddComponent<Animator>();
	animator->SetAnimation(*ResourceManager::GetAnimation("player/idle"));

	HealthComponent* health = AddComponent<HealthComponent>();
	ReStart();
}

void Player::Start()
{
	gm = GetOwner()->GetObjectByName<GameManager>("GameManager");

	currentWeaponTypeUI->ChangeWeaponState("Attack");
	//currentWeaponTypeUI->transform->SetParent(transform);
	currentWeaponTypeUI->renderer->enable = false;
	currentWeaponTypeUI->SetDrawingPriority(OrderLayer::Player);


	if (gm->GetCurrentStage() <= 2)
	{
		PlayeTutorialrState* bt = new PlayeTutorialrState();
		bt->Init();
		bt->SetOwner(this);
		playerState = bt;
	}
	else
	{
		PlayerState* bt = new PlayerState();
		bt->Init();
		bt->SetOwner(this);
		playerState = bt;
	}

}

void Player::Update()
{

	//GetComponent<CircleCollider>()->DrawBounds();
	if (!gm->IsGameEnd())
	{
		playerState->Behave(Time::GetDeltaTime());
	}
	
	if (!isTutorial && isTutorialBT)
	{
		isTutorialBT = false;
		delete playerState;
		PlayerState* bt = new PlayerState();
		bt->Init();
		bt->SetOwner(this);
		playerState = bt;
	}
}
void Player::OnDestroy()
{
	delete playerState;
	ResourceManager::DeleteAnimation("player/idle");
	ResourceManager::DeleteAnimation("player/Run");
	ResourceManager::DeleteAnimation("UI/Mouse");
}

void Player::ReStart()
{
	HealthComponent* health = GetComponent<HealthComponent>();
	health->Init(5, &onPlayerHit, &onPlayerDie);
	health->SetUIYPos(renderer->GetHeight());
}

void Player::StateChanged()
{
	// 대충 게임 매니저에 알려주기
}
bool Player::CanMove(int index)
{
	if (index < 0 || index >= TILE_COUNT) return false;

	return gm->CanMove(index);
}

bool Player::IsAimPhase()
{
	return phase == PlayerPhase::Aim;
}

void Player::SetAttackTarget(GameObject* target)
{
	if (phase != PlayerPhase::Aim) return;
	if (target->type == EType::Structure && currentWeapon == WeaponType::Crossbow)
	{
		AudioFramework::Play_BankSound("Sound", "UI_Wrongtarget");
		return;
	}
	if (target->type == EType::Enemy)
	{
		MonsterComponent* mobComp = target->GetComponent<MonsterComponent>();

		if (mobComp->GetEnemyType() == EnemyType::Boss)
		{
			if (currentWeapon == WeaponType::Crossbow) return;

			Boss* boss = static_cast<Boss*>(target);
			if (boss->IsHandAlive()) return;
		}
		else if (mobComp->GetEnemyType() == EnemyType::Boss_Left)
		{
			if (currentWeapon == WeaponType::Crossbow) return;

			BossLeftHand* leftHand = static_cast<BossLeftHand*>(target);
		}
		else if (mobComp->GetEnemyType() == EnemyType::Boss_Right)
		{
			if (currentWeapon == WeaponType::Crossbow) return;

			BossRightHand* leftHand = static_cast<BossRightHand*>(target);
		}
	}

	attackTarget = target;
	// 나중에 어떻게 바뀔 지 모르니까 일단 AttackType이랑 WeaponType 구분함
	if (currentWeapon == WeaponType::Gun) attackType = AttackType::Normal;
	else if (currentWeapon == WeaponType::Crossbow) attackType = AttackType::KnockBack;

	Shot();
}
//void Player::SetKnockbackTarget(GameObject* target)
//{
//	if (phase == PlayerPhase::Aim)
//	{
//		attackTarget = target;
//		attackType = AttackType::KnockBack;
//		static_cast<Player_Aim*>(FSM->GetCurrentState())->Shot();
//	}
//}

void Player::SwapWeaponType()
{
	// 여기서 예외 체크?
	// 대충 스왑 애니메이션

	if (!Input::IsDown(KeyCode::R)) return;
	AudioFramework::Play_BankSound("Sound", "SFX_Weaponswap");

	if (currentWeapon == WeaponType::Gun)
	{
		currentWeaponTypeUI->ChangeWeaponState("KnockBack");
		currentWeapon = WeaponType::Crossbow;
		gm->cursor_aim->SetCrossbowCursor();
	}
	else if (currentWeapon == WeaponType::Crossbow)
	{
		currentWeaponTypeUI->ChangeWeaponState("Attack");
		currentWeapon = WeaponType::Gun;
		gm->cursor_aim->SetGunCursor();
	}
}
void Player::UpdateSprite(std::string key)
{
	renderer->SetImage(key);

	if (key == "player/idle")
	{
		renderer->anchor = Vector2{ -21.0f, 0.0f };
	}
	else if (key == "player/aim")
	{
		renderer->anchor = Vector2{ 6.0f, 0.0f };
	}
	else if (key == "player/attack")
	{
		renderer->anchor = Vector2{ 11.0f, 0.0f };
	}
	else if (key == "player/knockback_attack")
	{
		renderer->anchor = Vector2{ 17.0f, 0.0f };
	}
	else if (key == "player/hit")
	{
		renderer->anchor = Vector2{ -16.0f, 0.0f };
	}
	else if (key == "player/dead")
	{
		renderer->anchor = Vector2{ 0.0f, 0.0f };
	}
}
bool Player::Attack()
{
	if (init_attack == false)
	{
		UpdateSprite("player/attack");
		renderer->SetSizeAuto();

		moveTargetTileIndex = attackTarget->GetCurrentTileIndex();
		int indexGap = moveTargetTileIndex - GetCurrentTileIndex();
		attackDir = 1;
		if (indexGap < 0) attackDir = -1;
		else if (indexGap > 0) attackDir = 1;

		if (attackDir == -1 && IsFaceRight() == true) FlipX();
		else if (attackDir == 1 && IsFaceRight() == false) FlipX();

		AudioFramework::Play_BankSound("Sound", "SFX_Bullet");
		init_attack = true;
	}

	attack_elapsedTime += Time::GetRealDeltaTime();
	if (attack_step == 0)
	{
		if (attack_elapsedTime >= GetConfig().Player_AttackEnterDelay)
		{
			// Effect Enter
			gameEffect = AddObject<GameEffect>();
			gameEffect->
				CreateEffect(
					"VFX/NormalAttack",
					Vector3(
						transform->GetPosition().x + (180.f * transform->GetScale().x),
						transform->GetPosition().y + 160.f,
						0)
				);
			gameEffect->transform->SetScaleX(transform->GetScale().x);
			gameEffect->GetComponent<SpriteRenderer>()->SetSize(330, 330);

			// Effect Enter
			SceneEffect::PlayerAttackEnter(GetConfig().Effect_EnterShakeMagnitudeOnPlayerAttack, GetConfig().Effect_EnterShakeDurationOnPlayerAttack);

			if (attackTarget->type == EType::Enemy)
			{
				attackTarget->PlayHitKnockbackMotion(attackDir, GetConfig().Common_HitKnockbackDistance, GetConfig().Common_HitKnockbackDuration);
			}
			else if (attackTarget->type == EType::Structure)
			{
				//static_cast<Trap*>(player->attackTarget)->OnClicked();
			}

			++attack_step;
			attack_elapsedTime -= GetConfig().Player_AttackEnterDelay;
		}
		return false;
	}
	else if (attack_step == 1)
	{
		if (attack_elapsedTime >= GetConfig().Player_AttackHitStopDuration)
		{
			// Effect Exit
			SceneEffect::PlayerAttackExit(GetConfig().Effect_ExitShakeMagnitudeOnPlayerAttack, GetConfig().Effect_ExitShakeDurationOnPlayerAttack);

			attackTarget->GetComponent<HealthComponent>()->Hit(GetConfig().Player_NormalAttackDamage);

			if (attackTarget->GetComponent<MonsterComponent>() != nullptr && attackTarget->GetComponent<MonsterComponent>()->GetEnemyType() == EnemyType::Boss) {
				static_cast<Boss*>(attackTarget)->CloseEye();
			}

			AudioFramework::Play_BankSound("Sound", "SFX_Bullet_Hit");

			++attack_step;
			attack_elapsedTime -= GetConfig().Player_AttackHitStopDuration;
		}
		return false;
	}
	else if (attack_step == 2)
	{
		if (attack_elapsedTime >= GetConfig().Player_AttackExitDelay)
		{
			phase = PlayerPhase::Idle;
			UpdateSprite("player/idle");

			animator->enable = true;


			renderer->SetSizeAuto();
			//renderer->SetSize(143, 200);

			attackTarget = nullptr;
			return true;
		}
		return false;
	}

	// 안전장치
	return false;
}

bool Player::KnockbackAttack()
{
	// 넉백의 경우 attackTarget의 EType이 Enemy임이 보장됨
	if (init_attack == false)
	{
		attack_elapsedTime = 0.0f;
		UpdateSprite("player/knockback_attack");
		renderer->SetSizeAuto();

		moveTargetTileIndex = attackTarget->GetCurrentTileIndex();
		int indexGap = attackTarget->GetCurrentTileIndex() - GetCurrentTileIndex();
		attackDir = 1;
		if (indexGap < 0) attackDir = -1;
		else if (indexGap > 0) attackDir = 1;

		if (attackDir == -1 && IsFaceRight() == true) FlipX();
		else if (attackDir == 1 && IsFaceRight() == false) FlipX();

		AudioFramework::Play_BankSound("Sound", "SFX_Arrow");
		init_attack = true;
	}

	attack_elapsedTime += Time::GetRealDeltaTime();
	if (attack_step == 0)
	{
		if (attack_elapsedTime >= GetConfig().Player_KnockbackEnterDelay)
		{
			// Effect Enter
			gameEffect = AddObject<GameEffect>();
			gameEffect->
				CreateEffect(
					"VFX/CrossbowAttack",
					Vector3(
						transform->GetPosition().x + (100.f * transform->GetScale().x),
						transform->GetPosition().y + 140.f,
						0)
				);
			gameEffect->transform->SetScaleX(transform->GetScale().x);
			gameEffect->GetComponent<SpriteRenderer>()->SetSize(300, 300);

			// 히트스톱은 넉백으로 인한 충돌이 발생했을 때
			//SceneEffect::PlayerAttackEnter(GetConfig().Effect_EnterShakeMagnitudeOnPlayerAttack, GetConfig().Effect_EnterShakeDurationOnPlayerAttack);

			++attack_step;
			attack_elapsedTime -= GetConfig().Player_KnockbackEnterDelay;
		}
		return false;
	}
	else if (attack_step == 1)
	{
		if (attack_elapsedTime >= GetConfig().Player_KnockbackHitStopDuration)
		{
			// 히트스톱은 넉백으로 인한 충돌이 발생했을 때
			//SceneEffect::PlayerAttackExit(GetConfig().Effect_ExitShakeMagnitudeOnPlayerAttack, GetConfig().Effect_ExitShakeDurationOnPlayerAttack);

			gm->Knockback(true, attackTarget, attackDir);
			AudioFramework::Play_BankSound("Sound", "SFX_Arrow_Hit");

			++attack_step;
			attack_elapsedTime -= GetConfig().Player_KnockbackHitStopDuration;
		}
		return false;
	}
	else if (attack_step == 2)
	{
		if (attack_elapsedTime >= GetConfig().Player_KnockbackExitDelay)
		{
			phase = PlayerPhase::Idle;
			UpdateSprite("player/idle");

			animator->enable = true;

			renderer->SetSizeAuto();
			//renderer->SetSize(143, 200);

			attackTarget = nullptr;
			return true;
		}
		return false;
	}

	// 안전장치
	return false;
}

void Player::Shot()
{
	phase = PlayerPhase::Attack;
}

void Player::TutorialEnd()
{
	isTutorial = false;
}















//
//
//// Tutorial
//void Player_TutorialMove::Initialize()
//{
//	player = static_cast<Player*>(owner);
//
//	Tutorial_Init();
//}
//void Player_TutorialMove::OnStateEnter()
//{
//	player->phase = PlayerPhase::Idle;
//	//player->renderer->SetImage("player/idle");
//	action_fixed = false;
//	t = 0.0f;
//	init = false;
//
//	player->gm->shade->SetEnable(false);
//	//player->gm->cursor_aim->SetDefaultCursor();
//
//	player->animator->enable = true;
//	player->renderer->SetSize(143, 200);
//
//	Tutorial_Enter();
//}
//void Player_TutorialMove::Update()
//{
//	// Tutorial_Action 호출하고 bool success 반환받으면 넘기기
//	bool state = Tutorial_Action();
//	if (state == false) return;
//
//	if (player->tutorial_turn >= 4)
//	{
//		// 이동 중 무기 스왑
//		player->SwapWeaponType();
//	}
//
//	if (action_fixed == false)
//	{
//		if (Input::IsDown(KeyCode::ArrowLeft) || Input::IsDown(KeyCode::A))
//		{
//			if (int tagetPos = player->GetCurrentTileIndex() - 1; player->gm->IsTileRangeIndex(tagetPos))
//			{
//				action_fixed = true;
//				player->moveType = MoveType::Left;
//				if (player->IsFaceRight() == true) player->FlipX();
//				player->targetIndex = tagetPos;
//				// 절반만 이동한 뒤 사격 모드로
//				initX = DEFINE::GetTilePositionX(player->GetCurrentTileIndex());			
//				targetX = (initX + DEFINE::GetTilePositionX(player->targetIndex)) / 2;
//				t = 0.0f;
//			}
//		}
//		else if (Input::IsDown(KeyCode::ArrowRight) || Input::IsDown(KeyCode::D))
//		{
//			if (int tagetPos = player->GetCurrentTileIndex() + 1; player->gm->IsTileRangeIndex(tagetPos))
//			{
//				action_fixed = true;
//				player->moveType = MoveType::Right;
//				if (player->IsFaceRight() == false) player->FlipX();
//				player->targetIndex = tagetPos;
//				initX = DEFINE::GetTilePositionX(player->GetCurrentTileIndex());
//				targetX = (initX + DEFINE::GetTilePositionX(player->targetIndex)) / 2;
//				t = 0.0f;
//			}
//		}
//		else if (Input::IsDown(KeyCode::ArrowUp) || Input::IsDown(KeyCode::W))
//		{
//			if (player->tutorial_turn <= 3) return;
//			// 튜토리얼에서는 사용 불가? 3턴 전까지만?
//			
//			player->moveType = MoveType::Stand;
//			player->targetIndex = player->GetCurrentTileIndex();
//			player->TutorialFSM->SetState<Player_TutorialAim>();
//		}
//	}
//	else if (action_fixed == true)
//	{
//		t += 1 / GetConfig().Player_FirstMoveDuration * Time::GetDeltaTime();
//		if (t < 1.0f)
//		{
//			float newX = Math::Lerp(initX, targetX, t);
//			player->transform->SetPositionX(newX);
//		}
//		else if (t >= 1.0f)
//		{
//			player->transform->SetPositionX(targetX);
//			player->TutorialFSM->SetState<Player_TutorialAim>();
//		}
//	}
//}
//void Player_TutorialMove::OnStateExit()
//{
//	player->animator->enable = false;
//	Tutorial_Exit();
//}
//
//bool Player_TutorialMove::Tutorial_Action()
//{
//	if (player->gm->GetCurrentPhase() != Phase::Player_Action) return false;
//
//	switch (player->tutorial_turn)
//	{
//	case 1:
//		return Tutorial_Turn1();
//	case 2:
//		return Tutorial_Turn2();
//	case 3:
//		return Tutorial_Turn3();
//	}
//
//	return true;
//}
//
//bool Player_TutorialMove::Tutorial_Turn1()
//{
//	if (t1_keyInfo->GetComponent<SpriteRenderer>()->color.A == 0.0f) turn1_init = false;
//	if (!turn1_init)
//	{
//		turn1_init = true;
//	}
//
//	return true;
//}
//
//bool Player_TutorialMove::Tutorial_Turn2()
//{
//	if (!turn2_init)
//	{
//		turn2_init = true;
//	}
//
//	if (t2_block_player_action_elapsed_time < t2_block_player_action_time)
//	{
//		t2_block_player_action_elapsed_time += Time::GetRealDeltaTime();
//		return false;
//	}
//	return true;
//}
//
//bool Player_TutorialMove::Tutorial_Turn3()
//{
//	if (!turn3_init)
//	{
//		turn3_init = true;
//	}
//
//	if (t3_block_player_action_elapsed_time < t3_block_player_action_time)
//	{
//		t3_block_player_action_elapsed_time += Time::GetRealDeltaTime();
//		return false;
//	}
//	else
//	{
//		if (turn3_init2 == false)
//		{
//			turn3_init2 = true;
//		}
//		return true;
//	}
//
//	return false;
//}
//
//
//void Player_TutorialAim::Initialize()
//{
//	player = static_cast<Player*>(owner);
//
//	Tutorial_Init();
//}
//void Player_TutorialAim::OnStateEnter()
//{
//	player->phase = PlayerPhase::Aim;
//	player->renderer->SetImage("player/aim");
//	player->renderer->SetSizeAuto();
//	player->gm->shade->SetEnable(true);
//	
//	if (player->tutorial_turn >= 3)
//	{
//		SpriteRenderer* curWeaponType = player->currentWeaponTypeUI->GetComponent<SpriteRenderer>();
//		curWeaponType->enable = true;
//		curWeaponType->color.A = 0.0f;
//		player->AddRealTimeTimerFunction([this, curWeaponType, t = 0.0f](bool lu) mutable
//			{
//				t += Time::GetRealDeltaTime() / 0.3f;
//				if (t >= 1.0f) t = 1.0f;
//				curWeaponType->color.A = Math::Lerp(0.0f, 1.0f, t);
//				if (lu)
//				{
//					curWeaponType->color.A = 1.0f;
//				}
//			}, true, 0.3f, 1);
//
//		player->currentWeaponTypeUI->transform->SetPosition(player->transform->GetPosition() + Vector3{ 0, 270, 0 });
//	}
//	
//	Tutorial_Enter();
//}
//void Player_TutorialAim::Update()
//{
//	bool state = Tutorial_Action();
//	if (state == false) return;
//
//	if (player->tutorial_turn >= 3)
//	{
//		// 에임 중 무기 스왑
//		player->SwapWeaponType();
//	}
//
//	// 이동 취소
//	switch (player->moveType)
//	{
//	case MoveType::Stand:
//		if (KeyDown(Esc) || KeyDown(A) || KeyDown(ArrowLeft) || KeyDown(D) || KeyDown(ArrowRight))
//		{
//			player->TutorialFSM->SetState<Player_TutorialMove>();
//		}
//		break;
//	case MoveType::Left:
//		if (KeyDown(Esc) || KeyDown(D) || KeyDown(ArrowRight) || KeyDown(W) || KeyDown(ArrowUp))
//		{
//			player->transform->SetPositionX(DEFINE::GetTilePositionX(player->GetCurrentTileIndex()));
//			player->TutorialFSM->SetState<Player_TutorialMove>();
//		}
//		break;
//	case MoveType::Right:
//		if (KeyDown(Esc) || KeyDown(A) || KeyDown(ArrowLeft) || KeyDown(W) || KeyDown(ArrowUp))
//		{
//			player->transform->SetPositionX(DEFINE::GetTilePositionX(player->GetCurrentTileIndex()));
//			player->TutorialFSM->SetState<Player_TutorialMove>();
//		}
//		break;
//	}
//}
//void Player_TutorialAim::OnStateExit()
//{
//	//player->gm->cursor_aim->SetDefaultCursor();
//
//	if (player->tutorial_turn >= 3)
//	{
//		SpriteRenderer* curWeaponType = player->currentWeaponTypeUI->GetComponent<SpriteRenderer>();
//		curWeaponType->enable = true;
//		curWeaponType->color.A = 0.0f;
//		player->AddRealTimeTimerFunction([this, curWeaponType, t = 0.0f](bool lu) mutable
//			{
//				t += Time::GetRealDeltaTime() / 0.3f;
//				if (t >= 1.0f) t = 1.0f;
//				curWeaponType->color.A = Math::Lerp(1.0f, 0.0f, t);
//				if (lu)
//				{
//					curWeaponType->color.A = 0.0f;
//					curWeaponType->enable = false;
//				}
//			}, true, 0.3f, 1);
//
//	}
//	Tutorial_Exit();
//}
//
//void Player_TutorialAim::Shot()
//{
//	if (player->tutorial_turn == 3 && turn3_press_init == false) return;
//
//	if (player->GetCurrentTileIndex() != player->targetIndex)
//	{
//		player->TutorialFSM->SetState<Player_TutorialFinishMove>();
//	}
//	else
//	{
//		player->TutorialFSM->SetState<Player_TutorialAttack>();
//	}
//	player->gm->shade->SetEnable(false);
//}
//
//void Player_TutorialAim::Tutorial_Init()
//{
//
//				"VFX/HitBlood1",
//
//
//}
//
//void Player_TutorialAim::Tutorial_Enter()
//{
//	if (player->tutorial_turn == 3)
//	{
//	}
//}
//
//void Player_TutorialAim::Tutorial_Exit()
//{
//	if (player->tutorial_turn == 1)
//	{
//	}
//
//	if (player->tutorial_turn == 2)
//	{
//
//	}
//}
//
//bool Player_TutorialAim::Tutorial_Action()
//{
//	switch (player->tutorial_turn)
//	{
//	case 1:
//		return Tutorial_Turn1();
//	case 2:
//		return Tutorial_Turn2();
//	case 3:
//		return Tutorial_Turn3();
//	}
//
//	return true;
//}
//
//bool Player_TutorialAim::Tutorial_Turn1()
//{
//	if (turn1_init == false)
//	{
//
//		turn1_init = true;
//	}
//
//	return true;
//}
//
//bool Player_TutorialAim::Tutorial_Turn2()
//{
//	if (turn2_init == false)
//	{
//		
//
//		turn2_init = true;
//	}
//
//	return true;
//}
//
//bool Player_TutorialAim::Tutorial_Turn3()
//{
//	if (turn3_init == false)
//	{
//		turn3_init = true;
//	}
//	if (turn3_press_init == false)
//	{
//		if (Input::IsDown(KeyCode::R))
//		{
//
//
//			turn3_press_init = true;
//		}
//	}
//	else
//	{
//		if (turn3_dialog_init == false)
//		{
//			
//
//			turn3_dialog_init = true;
//		}
//		if (t3_block_player_action_elapsed_time < t3_block_player_action_time)
//		{
//			t3_block_player_action_elapsed_time += Time::GetRealDeltaTime();
//			return false;
//		}
//		else
//		{
//			if (turn3_dialog_init2 == false)
//			{
//				player->AddRealTimeTimerFunction([this](bool lu)
//					{
//						player->AddRealTimeTimerFunction([this](bool lu)
//							{
//								t3_dialog2->GetComponent<SpriteRenderer>()->color.A -= Time::GetDeltaTime() / t3_dialog2ShowingDuration;
//								if (lu)
//								{
//									t3_dialog2->GetComponent<SpriteRenderer>()->color.A = 0.0f;
//									t3_dialog2->SetEnable(false);
//
//									player->AddRealTimeTimerFunction([this](bool lu)
//										{
//											t4_dialog1->SetEnable(true);
//											player->AddRealTimeTimerFunction([this](bool lu)
//												{
//													t4_dialog1->GetComponent<SpriteRenderer>()->color.A += Time::GetDeltaTime() / t4_dialog1ShowingDuration;
//													if (lu)
//													{
//														t4_dialog1->GetComponent<SpriteRenderer>()->color.A = 1.0f;
//													}
//												}, true, t4_dialog1ShowingDuration, 1);
//										}, false, t4_dialog1ShowingInterval, 1);
//
//									player->AddRealTimeTimerFunction([this](bool lu)
//										{
//											player->AddRealTimeTimerFunction([this](bool lu)
//												{
//													t4_dialog1->GetComponent<SpriteRenderer>()->color.A -= Time::GetDeltaTime() / t4_dialog1ShowingDuration;
//													if (lu)
//													{
//														t4_dialog1->GetComponent<SpriteRenderer>()->color.A = 0.0f;
//														t4_dialog1->SetEnable(false);
//													}
//												}, true, t4_dialog1ShowingDuration, 1);
//										}, false, t4_dialog1ShowingInterval + t4_dialog1ShowedDuration, 1);
//								}
//							}, true, t3_dialog2ShowingDuration, 1);
//
//						
//					}, false, t3_dialog2_showedDuration, 1);
//				turn3_dialog_init2 = true;
//			}
//			return true;
//		}
//		
//	}
//	
//	return false;
//}
//
//
//struct PlayerTutorialMoveCommand : public ICommand
//{
//	virtual bool Do() override
//	{
//		return _this->Move();
//	}
//	Player_TutorialFinishMove* _this;
//};
//
//void Player_TutorialFinishMove::Initialize()
//{
//	player = static_cast<Player*>(owner);
//}
//void Player_TutorialFinishMove::OnStateEnter()
//{
//	//player->phase = PlayerPhase::FinishMove;
//
//	PlayerTutorialMoveCommand* playerMoveCommand = new PlayerTutorialMoveCommand();
//	playerMoveCommand->_this = this;
//
//	// 실제 이동
//	player->MoveToTile({ .tileIndex = player->targetIndex, .duration = GetConfig().Player_SecondMoveDuration, .easing = Math::EaseOutCubic, .isSwap = true });
//
//	// 이건 그냥 moveDuration동안 커맨드 대기하기
//	player->gm->RegisterCommandList(ActSpeed::PlayerMove, playerMoveCommand);
//
//	player->TutorialFSM->SetState<Player_TutorialAttack>();
//	player->renderer->SetImage("player/idle");
//	player->renderer->SetSizeAuto();
//
//	t = 0.0f;
//
//	player->currentWeaponTypeUI->GetComponent<SpriteRenderer>()->enable = false;
//
//}
//
//void Player_TutorialFinishMove::Update()
//{
//
//}
//void Player_TutorialFinishMove::OnStateExit()
//{
//}
//
//bool Player_TutorialFinishMove::Move()
//{
//	t += 1 * Time::GetDeltaTime() / GetConfig().Player_SecondMoveDuration;
//	if (t < 1.0f)
//	{
//		// 이제 이동 애니메이션은 MoveToTile에서
//		// float newX = Math::Lerp(initX, targetX, t);
//		// player->transform->SetPositionX(newX);
//		return false;
//	}
//	else if (t >= 1.0f)
//	{
//
//		return true;
//	}
//}
//
//
//
//
//void Player_TutorialAttack::Initialize()
//{
//	player = static_cast<Player*>(owner);
//}
//void Player_TutorialAttack::OnStateEnter()
//{
//	//player->phase = PlayerPhase::Attack;
//	player->init_attack = false;
//	player->attack_elapsedTime = 0.0f;
//	player->attack_step = 0;
//
//	player->currentWeaponTypeUI->GetComponent<SpriteRenderer>()->enable = false;
//
//	if (player->attackType == AttackType::Normal)
//	{
//		// 스프라이트 변경은 Attack, KnockbackAttack에서
//		PlayerAttackCommand* playerAttackCommand = new PlayerAttackCommand();
//		playerAttackCommand->_this = player;
//		player->gm->RegisterCommandList(ActSpeed::PlayerAttack, playerAttackCommand);
//	}
//	else if (player->attackType == AttackType::KnockBack)
//	{
//		// 스프라이트 변경은 Attack, KnockbackAttack에서
//		PlayerKnockbackCommand* playerKnockbackCommand = new PlayerKnockbackCommand();
//		playerKnockbackCommand->_this = player;
//		player->gm->RegisterCommandList(ActSpeed::PlayerAttack, playerKnockbackCommand);
//	}
//
//	// 다음 페이즈로 넘어가면 등록된 커맨드 true를 반환할 때까지 반복 호출
//	player->gm->NextPhase();
//}
//void Player_TutorialAttack::Update()
//{
//
//}
//void Player_TutorialAttack::OnStateExit()
//{
//	player->tutorial_turn++;
//}