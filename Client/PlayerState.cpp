#include "PlayerState.h"
#include "Player.h"
#include "GameManager.h"
#include "Cursor_Aim.h"
#include "WeaponStateUI.h"
#include "ICommand.h"
#include "Trap.h"
#include "ChatBalloonComponent.h"
#include "PlayerText.h"
#include "Dialog.h"
#include "TacticModeBtn.h"
#include "MonsterComponent.h"
#include "Highlightable.h"
#include "MonsterComponent.h"`


struct PlayerMoveCommand : public ICommand
{
	virtual bool Do() override
	{
		if (!isInit)
		{
			isInit = true;
			owner->MoveToTile({ .tileIndex = owner->moveTargetTileIndex,.easing = Math::EaseOutCubic, .duration = GetConfig().Player_SecondMoveDuration,  .isSwap = true , .isFinish = &isMoveFinish });
		}

		return isMoveFinish;
	}
	bool isMoveFinish;
	bool isInit;
	Player* owner;
};

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



#define KeyDown(x) (Input::IsDown(KeyCode::x))
void PlayerState::Init()
{
	Archive archive;
	
	{
		YAML::Emitter emitter;
		emitter << YAML::BeginSeq;

		emitter << LoadArchive("PlayerWait:");
		emitter << LoadArchive("PlayerIdle:");
		emitter << LoadArchive("PlayerAim:");

		emitter << YAML::EndSeq;

		archive["rootNode"]["BT::SequenceNode"]["childNodes"] = LoadArchive(emitter.c_str());
	}

	std::stringstream tempsstream;
	tempsstream << archive;
	//auto temp = tempsstream.str();
	DeSerialize(LoadArchive(tempsstream));
}

void PlayeTutorialrState::Init()
{
	Archive archive;

	{
		YAML::Emitter emitter;
		emitter << YAML::BeginSeq;

		emitter << LoadArchive("PlayerTutorialWait:");
		emitter << LoadArchive("PlayerTutorialIdle:");
		emitter << LoadArchive("PlayerTutorialAim:");

		emitter << YAML::EndSeq;

		archive["rootNode"]["BT::SequenceNode"]["childNodes"] = LoadArchive(emitter.c_str());
	}

	std::stringstream tempsstream;
	tempsstream << archive;
	auto temp = tempsstream.str();
	DeSerialize(LoadArchive(tempsstream));

}

void PlayeTutorialrState::NextTutorialState()
{
	tutorialPhase = ETutorialPhase::Type(tutorialPhase + 1);
	if (tutorialPhase >= ETutorialPhase::TutorialEnd)
	{
		GetOwner<Player>()->isTutorial = false;
		tutorialPhase = ETutorialPhase::TutorialEnd;
	}
}

class PlayerIdle : public BT::ActionNode
{
#define PlayerIdleReflect(x)
	GENERATED_BODY(PlayerIdle, BT::ActionNode)
public:
	virtual void OnEnter() override
	{
		owner = GetOwner<Player>();

		owner->phase = PlayerPhase::Idle;
		action_fixed = false;
		t = 0.0f;
		init = false;

		owner->moveTargetTileIndex = owner->GetCurrentTileIndex();

		owner->gm->shade->SetEnable(false);

		owner->animator->enable = true;

		owner->UpdateSprite("player/idle");
		owner->renderer->SetSizeAuto();

		owner->currentWeaponTypeUI->GetComponent<SpriteRenderer>()->enable = false;

	}

	virtual void OnExit() override
	{
		owner->animator->enable = false;
	}

	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		if (owner->gm->GetCurrentPhase() <= Phase::MonsterSpawn)
		{
			if (owner->IsFaceRight() == false) owner->FlipX();
		}
		owner->SwapWeaponType();

		if (action_fixed == false)
		{
			if (Input::IsDown(KeyCode::ArrowLeft) || Input::IsDown(KeyCode::A))
			{
				if (int targetIndex = owner->GetCurrentTileIndex() - 1; owner->gm->IsTileRangeIndex(targetIndex))
				{
					other = owner->gm->GetTile(targetIndex)->object;
					if (other != nullptr && other->GetComponent<MonsterComponent>()->IsBossMonster()) return BT::Status::Running;

					action_fixed = true;
					owner->moveType = MoveType::Left;
					if (owner->IsFaceRight() == true) owner->FlipX();
					owner->moveTargetTileIndex = targetIndex;
					// 절반만 이동한 뒤 사격 모드로
					initX = DEFINE::GetTilePositionX(owner->GetCurrentTileIndex());
					targetX = (initX + DEFINE::GetTilePositionX(owner->moveTargetTileIndex)) * 0.5f;
					t = 0.0f;

					if (other != nullptr)
					{
						isSwap = true;
						other_initX = DEFINE::GetTilePositionX(targetIndex);
						float tt = 0.2f;
						other_targetX = other_initX * (1 - tt) + initX * tt;
					}
					else
					{
						isSwap = false;
					}
				}
			}
			else if (Input::IsDown(KeyCode::ArrowRight) || Input::IsDown(KeyCode::D))
			{
				if (int targetIndex = owner->GetCurrentTileIndex() + 1; owner->gm->IsTileRangeIndex(targetIndex))
				{
					other = owner->gm->GetTile(targetIndex)->object;
					if (other != nullptr && other->GetComponent<MonsterComponent>()->IsBossMonster()) return BT::Status::Running;

					action_fixed = true;
					owner->moveType = MoveType::Right;
					if (owner->IsFaceRight() == false) owner->FlipX();
					owner->moveTargetTileIndex = targetIndex;
					initX = DEFINE::GetTilePositionX(owner->GetCurrentTileIndex());
					targetX = (initX + DEFINE::GetTilePositionX(owner->moveTargetTileIndex)) / 2;
					t = 0.0f;

					other = owner->gm->GetTile(targetIndex)->object;
					if (other != nullptr)
					{
						isSwap = true;
						other_initX = DEFINE::GetTilePositionX(targetIndex);
						float tt = 0.2f;
						other_targetX = other_initX * (1 - tt) + initX * tt;
					}
					else
					{
						isSwap = false;
					}
				}
			}
			else if (Input::IsDown(KeyCode::ArrowUp) || Input::IsDown(KeyCode::W))
			{
				//action_fixed = true;
				owner->moveType = MoveType::Stand;
				owner->moveTargetTileIndex = owner->GetCurrentTileIndex();
				return BT::Status::Success;
			}
		}
		else if (action_fixed == true)
		{
			t += 1 / GetConfig().Player_FirstMoveDuration * Time::GetDeltaTime();
			if (t < 1.0f)
			{
				float newX = Math::Lerp(initX, targetX, t);
				owner->transform->SetPositionX(newX);

				if (isSwap == true)
				{
					float other_newX = Math::Lerp(other_initX, other_targetX, t);
					other->transform->SetPositionX(other_newX);
				}
			}
			else if (t >= 1.0f)
			{
				owner->transform->SetPositionX(targetX);

				if (isSwap == true)
				{
					other->transform->SetPositionX(other_targetX);
				}

				return BT::Status::Success;
			}
		}
		return BT::Status::Running;
	}
	Player* owner;
	bool action_fixed = false;
	float initX = 0.0f;
	float targetX = 0.0f;
	float t = 0.0f;
	bool init = false;

	// 스왑할 경우 상대방도 이동
	bool isSwap = false;
	GameObject* other;
	float other_initX = 0.0f;
	float other_targetX = 0.0f;
};

class PlayerAim : public BT::ActionNode
{
#define PlayerAimReflect(x)
	GENERATED_BODY(PlayerAim, BT::ActionNode)
public:
	virtual void OnEnter() override
	{
		owner = GetOwner<Player>();
		isMoveCancled = false;

		owner->phase = PlayerPhase::Aim;
		owner->UpdateSprite("player/aim");
		owner->renderer->SetSizeAuto();
		owner->gm->shade->SetEnable(true);


		SpriteRenderer* curWeaponType = owner->currentWeaponTypeUI->GetComponent<SpriteRenderer>();
		curWeaponType->enable = true;
		curWeaponType->color.A = 0.0f;
		owner->AddRealTimeTimerFunction([this, curWeaponType, t = 0.0f](bool lu) mutable
										 {
											 t += Time::GetRealDeltaTime() / 0.3f;
											 if (t >= 1.0f) t = 1.0f;
											 curWeaponType->color.A = Math::Lerp(0.0f, 1.0f, t);
											 if (lu)
											 {
												 curWeaponType->color.A = 1.0f;
											 }
										 }, true, 0.3f, 1);


		owner->currentWeaponTypeUI->transform->SetPosition(owner->transform->GetPosition() + Vector3{ 0, 270, 0 });
		// currentWeaponTypeUI를 기본 비강조로 사용할 때는 아래 코드 사용
		//player->currentWeaponTypeUI->transform->SetPosition( 0, 270, 0 );
		//if (player->transform->GetScale().x < 0) player->currentWeaponTypeUI->transform->SetScaleX(-1);
		//else if (player->transform->GetScale().x > 0) player->currentWeaponTypeUI->transform->SetScaleX(1);
		
		/*if (player->currentWeapon == WeaponType::Gun)
		{
			player->gm->cursor_aim->SetGunCursor();
		}
		else if (player->currentWeapon == WeaponType::Crossbow)
		{
			player->gm->cursor_aim->SetCrossbowCursor();
		}*/
	}

	virtual void OnExit() override
	{
		if (owner->phase != PlayerPhase::Attack) return;
		owner->gm->shade->SetEnable(false);

		SpriteRenderer* curWeaponType = owner->currentWeaponTypeUI->GetComponent<SpriteRenderer>();
		curWeaponType->enable = true;
		curWeaponType->color.A = 0.0f;
		owner->AddRealTimeTimerFunction([this, curWeaponType, t = 0.0f](bool lu) mutable
										 {
											 t += Time::GetRealDeltaTime() / 0.3f;
											 if (t >= 1.0f) t = 1.0f;
											 curWeaponType->color.A = Math::Lerp(1.0f, 0.0f, t);
											 if (lu)
											 {
												 curWeaponType->color.A = 0.0f;
												 curWeaponType->enable = false;
											 }
										 }, true, 0.3f, 1);


		PlayerMoveCommand* playerMoveCommand = new PlayerMoveCommand();
		playerMoveCommand->owner = owner;

		owner->gm->RegisterCommandList(ActSpeed::PlayerMove, playerMoveCommand);


		owner->init_attack = false;
		owner->attack_elapsedTime = 0.0f;
		owner->attack_step = 0;

		owner->currentWeaponTypeUI->GetComponent<SpriteRenderer>()->enable = false;

		if (owner->attackType == AttackType::Normal)
		{
			// 스프라이트 변경은 Attack, KnockbackAttack에서
			PlayerAttackCommand* playerAttackCommand = new PlayerAttackCommand();
			playerAttackCommand->_this = owner;
			owner->gm->RegisterCommandList(ActSpeed::PlayerAttack, playerAttackCommand);
		}
		else if (owner->attackType == AttackType::KnockBack)
		{
			// 스프라이트 변경은 Attack, KnockbackAttack에서
			PlayerKnockbackCommand* playerKnockbackCommand = new PlayerKnockbackCommand();
			playerKnockbackCommand->_this = owner;
			owner->gm->RegisterCommandList(ActSpeed::PlayerAttack, playerKnockbackCommand);
		}

		// 다음 페이즈로 넘어가면 등록된 커맨드 true를 반환할 때까지 반복 호출
		owner->gm->NextPhase();
	}

	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		// 에임 중 무기 스왑
		owner->SwapWeaponType();
		return CancleMove(deltaTime);
	}

	BT::Status::Type CancleMove(float deltaTime)
	{
		// 이동 취소
		switch (owner->moveType)
		{
		case MoveType::Stand:
			if (KeyDown(Esc) || KeyDown(A) || KeyDown(ArrowLeft) || KeyDown(D) || KeyDown(ArrowRight) || KeyDown(S) || KeyDown(ArrowDown))
			{
				owner->phase = PlayerPhase::Idle;
				isMoveCancled = true;
				return BT::Status::Failure;
			}
			break;
		case MoveType::Left:
			if (KeyDown(Esc) || KeyDown(D) || KeyDown(ArrowRight) || KeyDown(W) || KeyDown(ArrowUp) || KeyDown(S) || KeyDown(ArrowDown))
			{
				owner->transform->SetPositionX(DEFINE::GetTilePositionX(owner->GetCurrentTileIndex()));
				owner->phase = PlayerPhase::Idle;
				isMoveCancled = true;

				if (GameObject* other = owner->gm->GetTile(owner->moveTargetTileIndex)->object; other != nullptr)
				{
					other->transform->SetPositionX(DEFINE::GetTilePositionX(owner->moveTargetTileIndex));
				}

				return BT::Status::Failure;
			}
			break;
		case MoveType::Right:
			if (KeyDown(Esc) || KeyDown(A) || KeyDown(ArrowLeft) || KeyDown(W) || KeyDown(ArrowUp) || KeyDown(S) || KeyDown(ArrowDown))
			{
				owner->transform->SetPositionX(DEFINE::GetTilePositionX(owner->GetCurrentTileIndex()));
				owner->phase = PlayerPhase::Idle;
				isMoveCancled = true;

				if (GameObject* other = owner->gm->GetTile(owner->moveTargetTileIndex)->object; other != nullptr)
				{
					other->transform->SetPositionX(DEFINE::GetTilePositionX(owner->moveTargetTileIndex));
				}

				return BT::Status::Failure;
			}
			break;
		}
		if (owner->phase == PlayerPhase::Attack)
		{
			return BT::Status::Success;
		}
		else
		{
			return BT::Status::Running;
		}
	}
	Player* owner;
	bool isMoveCancled;
	
};

class PlayerWait : public BT::ActionNode
{
#define PlayerWaitReflect(x)
	GENERATED_BODY(PlayerWait, BT::ActionNode)
public:
	virtual void OnEnter() override
	{
		owner = GetOwner<Player>();
	}
	virtual void OnExit() override
	{

	}

	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		if (owner->gm->GetCurrentPhase() == Phase::Player_Action)
		{
			return BT::Status::Success;
		}
		else
		{
			return BT::Status::Running;
		}
	}
	Player* owner;
};

template <typename T>
void FadeIn(T* object, float duration = 0.4f, float resultAlpha = 1.0f)
{
	if (!object) return;
	object->SetEnable(true);
	object->GetComponent<SpriteRenderer>()->color.A = 0.0f;
	object->AddRealTimeTimerFunction([=](bool lu)
									{
										object->GetComponent<SpriteRenderer>()->color.A += (Time::GetDeltaTime() / duration) * resultAlpha;
										if (lu)
										{
											object->GetComponent<SpriteRenderer>()->color.A = resultAlpha;
										}
									}, true, duration, 1);
}

template <typename T>
void FadeOut(T* object, float duration = 0.4f)
{
	if (!object) return;
	object->SetEnable(true);
	object->GetComponent<SpriteRenderer>()->color.A = 1.0f;
	object->AddRealTimeTimerFunction([=](bool lu)
									{
										 object->GetComponent<SpriteRenderer>()->color.A -= Time::GetDeltaTime() / duration;
										if (lu)
										{
											object->Kill();
										}
									}, true, duration, 1);
}

class PlayerTutorialIdle : public PlayerIdle
{
#define PlayerTutorialIdleReflect(x)
	GENERATED_BODY(PlayerTutorialIdle, PlayerIdle)
public:
	virtual void OnEnter() override
	{
		PlayerIdle::OnEnter();
		ownerbt = GetBehaviourTree<PlayeTutorialrState>();
		elapsedTime = 0.0f;
		waitTime = 0.0f;
		isPressedButton = false;


		if (ownerbt->GetTutorialState() == ETutorialPhase::S1T1Start)
		{
			float t1_keyInfoShowingDuration = 0.7f;

			//t1_keyInfo = owner->AddUIObject<Object>("t1_keyInfo");
			//t1_keyInfo->transform->SetPositionY(-220.0f);
			//t1_keyInfo->SetEnable(false);
			//SpriteRenderer* renderer = t1_keyInfo->AddComponent<SpriteRenderer>();
			//renderer->SetImage("Tuto/T1/Move");

			//renderer->SetSize(300, 100);
			//t1_keyInfo->transform->SetPositionX(owner->transform->GetPosition().x);

			//FadeIn(t1_keyInfo, t1_keyInfoShowingDuration);



			{
				AButton = owner->AddUIObject<Object>();
				AButton->SetEnable(false);
				AButton->SetDrawingPriority(999);
				Vector3 playerPos = owner->transform->GetPosition();
				AButton->transform->SetPosition(playerPos.x - 120.0f, playerPos.y + 70.0f, 0.0f);
				SpriteRenderer* renderer = AButton->AddComponent<SpriteRenderer>();
				renderer->SetImage("Tuto/UI/Abutton");
				renderer->SetSizeAuto();

				FadeIn(AButton, s2t1AButtonShowingDuration, 0.8f);
			}


			{
				DButton = owner->AddUIObject<Object>();
				DButton->SetEnable(false);
				DButton->SetDrawingPriority(999);

				Vector3 playerPos = owner->transform->GetPosition();
				DButton->transform->SetPosition(playerPos.x + 130.0f, playerPos.y + 70.0f, 0.0f);
				SpriteRenderer* renderer = DButton->AddComponent<SpriteRenderer>();
				renderer->SetImage("Tuto/UI/Dbutton");
				renderer->SetSizeAuto();
				FadeIn(DButton, s2t1AButtonShowingDuration, 0.8f);
			}

		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1T2Start)
		{

			waitTime = 1.0f;

			shade = owner->AddUIObject<Object>("t2_shade");
			shade->SetEnable(false);
			shade->SetDrawingPriority(-1);
			SpriteRenderer* renderer = shade->AddComponent<SpriteRenderer>();
			renderer->SetImage("Tuto/T2/Shade");
			renderer->color.A = 0.0f;
			renderer->SetSizeAuto();

			shade->SetEnable(true);

			auto temp_t2_shade = shade;
			owner->AddRealTimeTimerFunction([=](bool lu) mutable
											 {
												 t2_shadeAlpha_t += Time::GetDeltaTime() / t2_shadeShowingDuration;
												 if (t2_shadeAlpha_t >= 1.0f) t2_shadeAlpha_t = 1.0f;
												 temp_t2_shade->GetComponent<SpriteRenderer>()->color.A = Math::Lerp(0.0f, t2_shadeMaxAlpha, t2_shadeAlpha_t);
											 }, true, t2_shadeShowingDuration, 1);

			
			t2_dialog1 = owner->AddUIObject<Dialog>("t2_dialog1");
			//t2_dialog1->spriteRenderer->SetImage("PopupUI");
			t2_dialog1->spriteRenderer->useNineSliced = true;
			t2_dialog1->spriteRenderer->nineSlicedImage = ResourceManager::GetNineSlicedImage("Tutorial/MessageBackground");

			t2_dialog1->SetText(L"악마가 있는 칸으로 이동 시,\n악마와 위치를 바꿀 수 있습니다.", 32);
			t2_dialog1->transform->SetPositionY(150.0f);


			t2_dialog1->textObject->GetComponent<TextRenderer>()->color.A = 0.0f;
			t2_dialog1->spriteRenderer->color.A = 0.0f;

			auto temp_t2_dialog1 = t2_dialog1;
			owner->AddRealTimeTimerFunction([=](bool lu)
											{
												temp_t2_dialog1->textObject->GetComponent<TextRenderer>()->color.A += Time::GetDeltaTime() / t2_canSwapPosMessageShowingDuration;
												temp_t2_dialog1->spriteRenderer->color.A += Time::GetDeltaTime() / t2_canSwapPosMessageShowingDuration;
												if (lu)
												{
													temp_t2_dialog1->textObject->GetComponent<TextRenderer>()->color.A = 1.0f;
													temp_t2_dialog1->spriteRenderer->color.A = 1.0f;
												}
											}, true, t2_canSwapPosMessageShowingDuration, 1);

		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1T3Start)
		{
			const PlayerText& text = PlayerTextManager().GetText(1);

			owner->GetComponent<ChatBalloonComponent>()->BubbleDialog(text.BubbleText, text.FontSize, text.BubbleDuration);



		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1End)
		{
			if(owner->gm->GetCurrentStage() <= 1)
			{
				if (!isS1EndPopUp)
				{
					isS1EndPopUp = true;

					t4_dialog1 = owner->AddUIObject<Dialog>("t2_dialog1");
					//t4_dialog1->spriteRenderer->SetImage("PopupUI");
					t4_dialog1->spriteRenderer->useNineSliced = true;
					t4_dialog1->spriteRenderer->nineSlicedImage = ResourceManager::GetNineSlicedImage("Tutorial/MessageBackground");


					t4_dialog1->SetText(L"자유롭게 연습해보세요.", 32);
					t4_dialog1->transform->SetPositionY(150.0f);


					t4_dialog1->textObject->GetComponent<TextRenderer>()->color.A = 0.0f;
					t4_dialog1->spriteRenderer->color.A = 0.0f;

					auto temp_t2_dialog1 = t4_dialog1;
					owner->AddRealTimeTimerFunction([=](bool lu)
													{
														temp_t2_dialog1->textObject->GetComponent<TextRenderer>()->color.A += Time::GetDeltaTime() / t2_canSwapPosMessageShowingDuration;
														temp_t2_dialog1->spriteRenderer->color.A += Time::GetDeltaTime() / t2_canSwapPosMessageShowingDuration;
														if (lu)
														{
															temp_t2_dialog1->textObject->GetComponent<TextRenderer>()->color.A = 1.0f;
															temp_t2_dialog1->spriteRenderer->color.A = 1.0f;
														}
													}, true, t2_canSwapPosMessageShowingDuration, 1);

				}

			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Start_1)
		{
			waitTime = 1.0f;
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T2Start)
		{
			WButton = owner->AddUIObject<Object>();
			WButton->SetEnable(false);
			WButton->SetDrawingPriority(999);

			Vector3 playerPos = owner->transform->GetPosition();
			//WButton->transform->SetPosition(playerPos.x + 100.0f, playerPos.x + 70.0f, 0.0f);
			WButton->transform->SetPosition(playerPos.x, playerPos.y + 270.0f, 0.0f);
			SpriteRenderer* renderer = WButton->AddComponent<SpriteRenderer>();
			renderer->SetImage("Tuto/UI/Wbutton");
			renderer->SetSizeAuto();



			FadeIn(WButton, s2t1AButtonShowingDuration);
			//UI팦업
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T3Start)
		{
			WButton = owner->AddUIObject<Object>();
			WButton->SetEnable(false);
			WButton->SetDrawingPriority(999);

			Vector3 playerPos = owner->transform->GetPosition();
			WButton->transform->SetPosition(playerPos.x, playerPos.x + 270.0f, 0.0f);
			SpriteRenderer* renderer = WButton->AddComponent<SpriteRenderer>();
			renderer->SetImage("Tuto/UI/Wbutton");
			renderer->SetSizeAuto();



			FadeIn(WButton, s2t1AButtonShowingDuration);

			waitTime = 1.0f;


			const PlayerText& text = PlayerTextManager().GetText(2);

			GetOwner<Player>()->GetComponent<ChatBalloonComponent>()->BubbleDialog(text.BubbleText, text.FontSize, text.BubbleDuration);
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T4Start)
		{
			{
				WButton = owner->AddUIObject<Object>();
				WButton->SetEnable(false);
				WButton->SetDrawingPriority(999);

				Vector3 playerPos = owner->transform->GetPosition();
				WButton->transform->SetPosition(playerPos.x, playerPos.y + 270.0f, 0.0f);
				SpriteRenderer* renderer = WButton->AddComponent<SpriteRenderer>();
				renderer->SetImage("Tuto/UI/Wbutton");
				renderer->SetSizeAuto();
	
				FadeIn(WButton, s2t1AButtonShowingDuration);
			}

			{
				AButton = owner->AddUIObject<Object>();
				AButton->SetEnable(false);
				AButton->SetDrawingPriority(999);
				Vector3 playerPos = owner->transform->GetPosition();
				AButton->transform->SetPosition(playerPos.x - 120.0f, playerPos.y + 70.0f, 0.0f);
				SpriteRenderer* renderer = AButton->AddComponent<SpriteRenderer>();
				renderer->SetImage("Tuto/UI/Abutton");
				renderer->SetSizeAuto();

				FadeIn(AButton, s2t1AButtonShowingDuration, 0.8f);
			}


			{
				DButton = owner->AddUIObject<Object>();
				DButton->SetEnable(false);
				DButton->SetDrawingPriority(999);

				Vector3 playerPos = owner->transform->GetPosition();
				DButton->transform->SetPosition(playerPos.x + 130.0f, playerPos.y + 70.0f, 0.0f);
				SpriteRenderer* renderer = DButton->AddComponent<SpriteRenderer>();
				renderer->SetImage("Tuto/UI/Dbutton");
				renderer->SetSizeAuto();
				FadeIn(DButton, s2t1AButtonShowingDuration, 0.8f);
			}
		}

	}
	virtual void OnExit() override
	{
		PlayerIdle::OnExit();

		if (ownerbt->GetTutorialState() == ETutorialPhase::S1T1Start)
		{
			FadeOut(AButton, s2t1AButtonShowingDuration);
			AButton = nullptr;
			FadeOut(DButton, s2t1AButtonShowingDuration);
			DButton = nullptr;
			//FadeOut(t1_keyInfo, t1_keyInfoShowingDuration);
			//t1_keyInfo = nullptr;
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1T2Start)
		{
			if (shade)
			{
				auto temp_t2_shade = shade;
				shade = nullptr;
				owner->AddRealTimeTimerFunction([=](bool lu) mutable
												{
													t2_shadeAlpha_t += Time::GetDeltaTime() / t2_shadeShowingDuration;
													if (t2_shadeAlpha_t >= 1.0f) t2_shadeAlpha_t = 1.0f;
													temp_t2_shade->GetComponent<SpriteRenderer>()->color.A = Math::Lerp(t2_shadeMaxAlpha, 0.0f, t2_shadeAlpha_t);
													if (lu)
													{
														temp_t2_shade->Kill();
													}
												}, true, t2_shadeShowingDuration, 1);
			}

			FadeOut(t2_dialog1, t2_canSwapPosMessageShowingDuration);
			t2_dialog1 = nullptr;
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1End)
		{
			FadeOut(t4_dialog1, 0.4f);
			t4_dialog1 = nullptr;
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Start_2_ING)
		{
			if (shade)
			{
				auto temp_t2_shade = shade;
				shade = nullptr;
				owner->AddRealTimeTimerFunction([=](bool lu) mutable
												{
													t2_shadeAlpha_t += Time::GetDeltaTime() / t2_shadeShowingDuration;
													if (t2_shadeAlpha_t >= 1.0f) t2_shadeAlpha_t = 1.0f;
													temp_t2_shade->GetComponent<SpriteRenderer>()->color.A = Math::Lerp(t2_shadeMaxAlpha, 0.0f, t2_shadeAlpha_t);
													if (lu)
													{
														temp_t2_shade->Kill();
													}
												}, true, t2_shadeShowingDuration, 1);
			}

			if (s2t1_dialog1)
			{
				FadeOut(s2t1_dialog1, t2_canSwapPosMessageShowingDuration);
				s2t1_dialog1 = nullptr;
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Start_4)
		{
			FadeOut(AButton, s2t1AButtonShowingDuration);
			AButton = nullptr;
			
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T2Start)
		{
			FadeOut(WButton, 0.4f);
			WButton = nullptr;
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T3Start)
		{
			FadeOut(WButton, 0.4f);
			WButton = nullptr;
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T4Start)
		{
			FadeOut(AButton, s2t1AButtonShowingDuration);
			AButton = nullptr;
			FadeOut(DButton, s2t1AButtonShowingDuration);
			DButton = nullptr;
			FadeOut(WButton, 0.4f);
			WButton = nullptr;
		}

		if (ownerbt->GetTutorialState() != ETutorialPhase::S1End)
		{
			ownerbt->NextTutorialState();
		}


	}
	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		if (ownerbt->GetTutorialState() < ETutorialPhase::S1T3Start && Input::IsDown(KeyCode::R))
		{
			return BT::Status::Running;
		}
		if (ownerbt->GetTutorialState() < ETutorialPhase::S2T2Start && (Input::IsDown(KeyCode::ArrowUp) || Input::IsDown(KeyCode::W)))
		{
			return BT::Status::Running;
		}

		if (owner->gm->GetCurrentStage() > 1 &&  ownerbt->GetTutorialState() <= ETutorialPhase::S1End)
		{
			if (owner->gm->GetCurrentPhase() == Phase::Player_Action)
			{
				while (ownerbt->tutorialPhase <= ETutorialPhase::S1End)
				{
					OnExit();
					ownerbt->NextTutorialState();
				}
				waitTime = 1.0f;
				if (isS1EndPopUp)
				{
					isS1EndPopUp = false;
				}
				isSkipExitOnce = false;
			}
			else if(!isSkipExitOnce)
			{
				isSkipExitOnce = true;
				OnExit();
			}
			
		}
		if (owner->gm->GetCurrentStage() > 2)
		{
			while (ownerbt->tutorialPhase < ETutorialPhase::TutorialEnd)
			{
				OnExit();
				ownerbt->NextTutorialState();
			}
			owner->isTutorial = false;
		}


		if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Start_1)
		{
			elapsedTime += Time::GetDeltaTime();
			if (elapsedTime > waitTime)
			{
				elapsedTime = 0.0f;
				ownerbt->NextTutorialState();
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Start_2)
		{
			elapsedTime += Time::GetDeltaTime();
			if (elapsedTime > waitTime)
			{
				shade = owner->AddUIObject<Object>("t2_shade");
				shade->SetEnable(false);
				shade->SetDrawingPriority(-1);
				SpriteRenderer* renderer = shade->AddComponent<SpriteRenderer>();
				renderer->SetImage("Tuto/T2/Shade");
				renderer->color.A = 0.0f;
				renderer->SetSizeAuto();


				shade->SetEnable(true);

				auto temp_t2_shade = shade;
				owner->AddRealTimeTimerFunction([=](bool lu) mutable
												{
													t2_shadeAlpha_t += Time::GetDeltaTime() / t2_shadeShowingDuration;
													if (t2_shadeAlpha_t >= 1.0f) t2_shadeAlpha_t = 1.0f;
													temp_t2_shade->GetComponent<SpriteRenderer>()->color.A = Math::Lerp(0.0f, t2_shadeMaxAlpha, t2_shadeAlpha_t);
												}, true, t2_shadeShowingDuration, 1);

				s2t1_dialog1 = owner->AddUIObject<Dialog>("s2t1_dialog1");
				//s2t1_dialog1->spriteRenderer->SetImage("PopupUI");
				s2t1_dialog1->spriteRenderer->useNineSliced = true;
				s2t1_dialog1->spriteRenderer->nineSlicedImage = ResourceManager::GetNineSlicedImage("Tutorial/MessageBackground");

				s2t1_dialog1->SetText(L"택티컬 모드 버튼을 눌러 모든 정보 표시를 한번에 끄고 켤 수 있습니다.", 32);
				s2t1_dialog1->transform->SetPositionY(150.0f);


				s2t1_dialog1->textObject->GetComponent<TextRenderer>()->color.A = 0.0f;
				s2t1_dialog1->spriteRenderer->color.A = 0.0f;

				auto temp_s2t1_dialog1 = s2t1_dialog1;
				owner->AddRealTimeTimerFunction([=](bool lu)
												{
													temp_s2t1_dialog1->textObject->GetComponent<TextRenderer>()->color.A += Time::GetDeltaTime() / t2_canSwapPosMessageShowingDuration;
													temp_s2t1_dialog1->spriteRenderer->color.A += Time::GetDeltaTime() / t2_canSwapPosMessageShowingDuration;
													if (lu)
													{
														temp_s2t1_dialog1->textObject->GetComponent<TextRenderer>()->color.A = 1.0f;
														temp_s2t1_dialog1->spriteRenderer->color.A = 1.0f;
													}
												}, true, t2_canSwapPosMessageShowingDuration, 1);



				ownerbt->NextTutorialState();


				tacticModeBtn = owner->GetOwner()->GetObjectByName<TacticModeBtn>("TacticModeBtn");
				taticbuttonOn = tacticModeBtn->on;
			}



		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Start_2_ING)
		{
			
			bool isTacticalOn = taticbuttonOn != tacticModeBtn->on;;
			if (isTacticalOn)
			{
				tacticModeBtn = nullptr;
				ownerbt->NextTutorialState();
				waitTime = 0.0f;

				if (shade)
				{
					auto temp_t2_shade = shade;
					shade = nullptr;
					owner->AddRealTimeTimerFunction([=](bool lu) mutable
													{
														t2_shadeAlpha_t += Time::GetDeltaTime() / t2_shadeShowingDuration;
														if (t2_shadeAlpha_t >= 1.0f) t2_shadeAlpha_t = 1.0f;
														temp_t2_shade->GetComponent<SpriteRenderer>()->color.A = Math::Lerp(t2_shadeMaxAlpha, 0.0f, t2_shadeAlpha_t);
														if (lu)
														{
															temp_t2_shade->Kill();
														}
													}, true, t2_shadeShowingDuration, 1);
				}

				if (s2t1_dialog1)
				{
					FadeOut(s2t1_dialog1, t2_canSwapPosMessageShowingDuration);
					s2t1_dialog1 = nullptr;
				}
			}
			if (Input::IsDown(KeyCode::R))
			{
				owner->SwapWeaponType();
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Start_3)
		{
			elapsedTime += Time::GetDeltaTime();
			if (elapsedTime > waitTime)
			{
				ownerbt->NextTutorialState();
				//UI팦업

				AButton = owner->AddUIObject<Object>();
				AButton->SetEnable(false);
				AButton->SetDrawingPriority(999);
				Vector3 playerPos = owner->transform->GetPosition();
				AButton->transform->SetPosition(playerPos.x -100.0f, playerPos.y + 70.0f, 0.0f);
				SpriteRenderer* renderer = AButton->AddComponent<SpriteRenderer>();
				renderer->SetImage("Tuto/UI/Abutton");
				renderer->color.A = 0.0f;
				renderer->SetSizeAuto();


				FadeIn(AButton, s2t1AButtonShowingDuration);
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Start_4)
		{
			if (!isPressedButton)
			{
				if (Input::IsDown(KeyCode::ArrowLeft) || Input::IsDown(KeyCode::A))
				{
					isPressedButton = true;
				}
				if (Input::IsDown(KeyCode::R))
				{
					owner->SwapWeaponType();
				}
			}
			if (isPressedButton)
			{
				return PlayerIdle::OnBehave(deltaTime);
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T2Start)
		{
			if (!isPressedButton)
			{
				if (Input::IsDown(KeyCode::ArrowUp) || Input::IsDown(KeyCode::W))
				{
					isPressedButton = true;

					FadeOut(s2t3_dialog, s2t1AButtonShowingDuration);
					s2t3_dialog = nullptr;
				}
				if (Input::IsDown(KeyCode::R))
				{
					owner->SwapWeaponType();
				}
			}

			if (isPressedButton)
			{
				return PlayerIdle::OnBehave(deltaTime);
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T3Start)
		{
			if (!isPressedButton)
			{
				if (Input::IsDown(KeyCode::ArrowUp) || Input::IsDown(KeyCode::W))
				{
					isPressedButton = true;
				}
				if (Input::IsDown(KeyCode::R))
				{
					owner->SwapWeaponType();
				}
			}

			if (isPressedButton)
			{
				return PlayerIdle::OnBehave(deltaTime);
			}
		}
		else
		{
			elapsedTime += Time::GetDeltaTime();
			if (elapsedTime > waitTime)
			{
				return PlayerIdle::OnBehave(deltaTime);
			}
		}


		return BT::Status::Running;
	}
	PlayeTutorialrState* ownerbt;
	Object* t1_keyInfo;
	Dialog* t2_dialog1;
	Object* shade;
	Object* AButton;
	Object* DButton;
	Object* WButton;
	Dialog* s2t3_dialog;
	Dialog* t4_dialog1;

	Dialog* s2t1_dialog1;

	TacticModeBtn* tacticModeBtn;
	bool taticbuttonOn;
	
	float waitTime;
	float elapsedTime;

	float t1_keyInfoShowingDuration = 0.7f;

	float t2_shadeAlpha_t = 0.0f;
	float t2_shadeShowingDuration = 0.4f;
	float t2_shadeMaxAlpha = 0.6f;
	float t2_canSwapPosMessageShowingDuration = 0.4f;

	float s2t1AButtonShowingDuration = 0.4f;

	bool isS1EndPopUp;
	bool isPressedButton;
	bool isSkipExitOnce;

};

class PlayerTutorialAim : public PlayerAim
{
#define PlayerTutorialAimReflect(x)
	GENERATED_BODY(PlayerTutorialAim, PlayerAim)
public:
	virtual void OnEnter() override
	{
		PlayerAim::OnEnter();
		ownerbt = GetBehaviourTree<PlayeTutorialrState>();
		elapsedTime = 0.0f;
		waitTime = 0.0f;
		owner->currentWeaponTypeUI->SetEnable(true);


		if (ownerbt->GetTutorialState() == ETutorialPhase::S1T1Aim)
		{
			GameObject* rightestMonster = nullptr;
			for (int i = owner->gm->InitTileSize - 1; i >= 0; i--)
			{
				TileObject* tile = owner->gm->GetTile(i);
				if (tile->object && tile->object->GetComponent<MonsterComponent>())
				{
					rightestMonster = tile->object;
					break;
				}
			}
			owner->currentWeaponTypeUI->SetEnable(false);


			if (rightestMonster)
			{
				clickThis = owner->AddUIObject<Object>();
				clickThis->transform->SetParent(rightestMonster->transform);
				clickThis->transform->SetPosition(100.0f, 100.0f, 0.f);
				SpriteRenderer* renderer = clickThis->AddComponent<SpriteRenderer>();
				Animator* animator = clickThis->AddComponent<Animator>();
				animator->SetAnimation(*ResourceManager::GetAnimation("UI/Mouse"));
				animator->enable = true;

				renderer->SetSizeAuto();

				FadeIn(clickThis, t1_clickThisShowingDuration);


				Object* clickThis2 = owner->AddUIObject<Object>();
				clickThis2->transform->SetParent(clickThis->transform);
				clickThis2->transform->SetPosition(-100.0f, -50.0f, 0.f);
				SpriteRenderer* renderer2 = clickThis2->AddComponent<SpriteRenderer>();
				renderer2->SetImage("UI/GunAim");
				renderer2->SetSizeAuto();
				renderer2->anchorType = AnchorType::BottomCenter;
				FadeIn(clickThis2, t1_clickThisShowingDuration, 0.6f);

				owner->GetOwner()->GetObjectByName<HighlightManager>("HighlightManager")->tutorial_target = rightestMonster;
			}
			

		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1T2Aim)
		{
			GameObject* rightestMonster = nullptr;
			for (int i = owner->gm->InitTileSize - 1; i >= 0; i--)
			{
				TileObject* tile = owner->gm->GetTile(i);
				if (tile->object && tile->object->GetComponent<MonsterComponent>())
				{
					rightestMonster = tile->object;
					break;
				}
			}
			owner->currentWeaponTypeUI->SetEnable(false);

			if (rightestMonster)
			{
				clickThis = owner->AddUIObject<Object>();
				clickThis->transform->SetParent(rightestMonster->transform);
				clickThis->transform->SetPosition(100.0f, 100.0f, 0.f);
				SpriteRenderer* renderer = clickThis->AddComponent<SpriteRenderer>();
				Animator* animator = clickThis->AddComponent<Animator>();
				animator->SetAnimation(*ResourceManager::GetAnimation("UI/Mouse"));
				animator->enable = true;

				renderer->SetSizeAuto();

				FadeIn(clickThis, t1_clickThisShowingDuration);

				Object* clickThis2 = owner->AddUIObject<Object>();
				clickThis2->transform->SetParent(clickThis->transform);
				clickThis2->transform->SetPosition(-100.0f, -50.0f, 0.f);
				SpriteRenderer* renderer2 = clickThis2->AddComponent<SpriteRenderer>();
				renderer2->SetImage("UI/GunAim");
				renderer2->SetSizeAuto();
				renderer2->anchorType = AnchorType::BottomCenter;
				FadeIn(clickThis2, t1_clickThisShowingDuration, 0.6f);

				owner->GetOwner()->GetObjectByName<HighlightManager>("HighlightManager")->tutorial_target = rightestMonster;
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1T3Aim_1)
		{
			waitTime = 1.0f;

			t3_pressR = owner->AddUIObject<Object>("t3_pressR");
			t3_pressR->transform->SetPosition(owner->transform->GetPosition() + Vector3(100.0f, 270.0f, 0.0f));
			SpriteRenderer* renderer = t3_pressR->AddComponent<SpriteRenderer>();
			renderer->SetImage("Tuto/T3/PressR");
			renderer->SetSizeAuto();


			FadeIn(t3_pressR, t3_pressRShowingDuration);




		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Aim) 
		{
			GameObject* rightestGunPowder = nullptr;
			for (int i = owner->gm->InitTileSize - 1; i >= 0; i--)
			{
				TileObject* tile = owner->gm->GetTile(i);
				if (tile->gunPowder)
				{
					rightestGunPowder = tile->gunPowder;
					break;
				}
			}
			if (rightestGunPowder)
			{
				clickThis = owner->AddUIObject<Object>();
				clickThis->transform->SetParent(rightestGunPowder->transform);
				clickThis->transform->SetPosition(100.0f, 100.0f, 0.f);
				SpriteRenderer* renderer = clickThis->AddComponent<SpriteRenderer>();
				Animator* animator = clickThis->AddComponent<Animator>();
				animator->SetAnimation(*ResourceManager::GetAnimation("UI/Mouse"));
				animator->enable = true;

				renderer->SetSizeAuto();

				FadeIn(clickThis, t1_clickThisShowingDuration);

				Object* clickThis2 = owner->AddUIObject<Object>();
				clickThis2->transform->SetParent(clickThis->transform);
				clickThis2->transform->SetPosition(-100.0f, -125.0f, 0.f);
				SpriteRenderer* renderer2 = clickThis2->AddComponent<SpriteRenderer>();
				renderer2->SetImage("UI/GunAim");
				renderer2->SetSizeAuto();
				renderer2->anchorType = AnchorType::BottomCenter;
				FadeIn(clickThis2, t1_clickThisShowingDuration, 0.6f);

				owner->GetOwner()->GetObjectByName<HighlightManager>("HighlightManager")->tutorial_target = rightestGunPowder;
			}

		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T2Aim)
		{

			t3_dialog2 = owner->AddUIObject<Dialog>("s2t3_dialog");
			t3_dialog2->spriteRenderer->SetImage("PopupUI");
			t3_dialog2->spriteRenderer->useNineSliced = true;
			t3_dialog2->spriteRenderer->nineSlicedImage = ResourceManager::GetNineSlicedImage("Tutorial/MessageBackground");
			t3_dialog2->SetText(L"악마가 소환될 위치에 올라가면 데미지를 입지만,\n악마 소환을 저지할 수 있습니다.", 32);
			t3_dialog2->transform->SetPositionY(150.0f);
			t3_dialog2->textObject->GetComponent<TextRenderer>()->color.A = 0.0f;
			t3_dialog2->spriteRenderer->color.A = 0.0f;
			auto temp_s2t3_dialog = t3_dialog2;
			owner->AddRealTimeTimerFunction([=](bool lu)
											{
												if (t3_dialog2)
												{
													temp_s2t3_dialog->textObject->GetComponent<TextRenderer>()->color.A += Time::GetDeltaTime() / t3_dialog2_showedDuration;
													temp_s2t3_dialog->spriteRenderer->color.A += Time::GetDeltaTime() / t3_dialog2_showedDuration;
													if (lu)
													{
														temp_s2t3_dialog->textObject->GetComponent<TextRenderer>()->color.A = 1.0f;
														temp_s2t3_dialog->spriteRenderer->color.A = 1.0f;
													}
												}
											}, true, t3_dialog2_showedDuration, 1);

			GameObject* rightesRush = nullptr;
			for (int i = owner->gm->InitTileSize - 1; i >= 0; i--)
			{
				TileObject* tile = owner->gm->GetTile(i);
				if (tile->object)
				{
					if (MonsterComponent* monster = tile->object->GetComponent<MonsterComponent>();
						monster && monster->GetEnemyType() == EnemyType::Rush)
					{
						rightesRush = tile->object;
						break;
					}
				}
			}

			if (rightesRush)
			{
				clickThis = owner->AddUIObject<Object>();
				clickThis->transform->SetParent(rightesRush->transform);
				clickThis->transform->SetPosition(100.0f, 100.0f, 0.f);
				SpriteRenderer* renderer = clickThis->AddComponent<SpriteRenderer>();
				Animator* animator = clickThis->AddComponent<Animator>();
				animator->SetAnimation(*ResourceManager::GetAnimation("UI/Mouse"));
				animator->enable = true;

				renderer->SetSizeAuto();

				FadeIn(clickThis, t1_clickThisShowingDuration);

				Object* clickThis2 = owner->AddUIObject<Object>();
				clickThis2->transform->SetParent(clickThis->transform);
				clickThis2->transform->SetPosition(-100.0f, -100.0f, 0.f);
				SpriteRenderer* renderer2 = clickThis2->AddComponent<SpriteRenderer>();
				renderer2->SetImage("UI/GunAim");
				renderer2->SetSizeAuto();
				renderer2->anchorType = AnchorType::BottomCenter;
				FadeIn(clickThis2, t1_clickThisShowingDuration, 0.6f);

				owner->GetOwner()->GetObjectByName<HighlightManager>("HighlightManager")->tutorial_target = rightesRush;
			}


		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T3Aim)
		{
			GameObject* righteschandelier = nullptr;
			for (size_t i = 0; i < owner->gm->InitTileSize; i++)
			{
				TileObject* tile = owner->gm->GetTile(i);
				if (tile->chandelier)
				{
					righteschandelier = tile->chandelier;
					break;
				}
			}

			if (righteschandelier)
			{
				clickThis = owner->AddUIObject<Object>();
				clickThis->transform->SetParent(righteschandelier->transform);
				clickThis->transform->SetPosition(100.0f, 100.0f, 0.f);
				SpriteRenderer* renderer = clickThis->AddComponent<SpriteRenderer>();
				Animator* animator = clickThis->AddComponent<Animator>();
				animator->SetAnimation(*ResourceManager::GetAnimation("UI/Mouse"));
				animator->enable = true;

				renderer->SetSizeAuto();

				FadeIn(clickThis, t1_clickThisShowingDuration);


				Object* clickThis2 = owner->AddUIObject<Object>();
				clickThis2->transform->SetParent(clickThis->transform);
				clickThis2->transform->SetPosition(-100.0f, -200.0f, 0.f);
				SpriteRenderer* renderer2 = clickThis2->AddComponent<SpriteRenderer>();
				renderer2->SetImage("UI/GunAim");
				renderer2->SetSizeAuto();
				renderer2->anchorType = AnchorType::BottomCenter;
				FadeIn(clickThis2, t1_clickThisShowingDuration, 0.6f);

				owner->GetOwner()->GetObjectByName<HighlightManager>("HighlightManager")->tutorial_target = righteschandelier;
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T4Aim)
		{
			t3_dialog2 = owner->AddUIObject<Dialog>("t3_dialog2");
			//t3_dialog2->spriteRenderer->SetImage("PopupUI");
			t3_dialog2->spriteRenderer->useNineSliced = true;
			t3_dialog2->spriteRenderer->nineSlicedImage = ResourceManager::GetNineSlicedImage("Tutorial/MessageBackground");

			t3_dialog2->SetText(L"아직 사격하지 않았다면\n S키를 눌러 이동을 취소할 수 있습니다.", 32);
			t3_dialog2->transform->SetPositionY(150.0f);


			t3_dialog2->textObject->GetComponent<TextRenderer>()->color.A = 0.0f;
			t3_dialog2->spriteRenderer->color.A = 0.0f;

			auto temp_t3_dialog2 = t3_dialog2;
			owner->AddRealTimeTimerFunction([=](bool lu)
											{
												temp_t3_dialog2->textObject->GetComponent<TextRenderer>()->color.A += Time::GetDeltaTime() / t3_pressRShowingDuration;
												temp_t3_dialog2->spriteRenderer->color.A += Time::GetDeltaTime() / t3_pressRShowingDuration;
												if (lu)
												{
													temp_t3_dialog2->textObject->GetComponent<TextRenderer>()->color.A = 1.0f;
													temp_t3_dialog2->spriteRenderer->color.A = 1.0f;
												}
											}, true, t3_pressRShowingDuration, 1);

			{
				SButton = owner->AddUIObject<Object>();
				SButton->SetEnable(false);
				SButton->SetDrawingPriority(999);

				Vector3 playerPos = owner->transform->GetPosition();
				SButton->transform->SetPosition(playerPos.x, playerPos.y - 70.0f, 0.0f);
				SpriteRenderer* renderer = SButton->AddComponent<SpriteRenderer>();
				renderer->SetImage("Tuto/UI/Sbutton");
				renderer->SetSizeAuto();

				FadeIn(SButton, t3_pressRShowingDuration);
			}

		}
	}
	virtual void OnExit() override
	{
		PlayerAim::OnExit();


		owner->GetOwner()->GetObjectByName<HighlightManager>("HighlightManager")->tutorial_target = nullptr;
		if (ownerbt->GetTutorialState() == ETutorialPhase::S1T1Aim)
		{
			FadeOut(clickThis, t1_clickThisShowingDuration);
			clickThis = nullptr;

		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1T2Aim)
		{
			FadeOut(clickThis, t1_clickThisShowingDuration);
			clickThis = nullptr;

		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1T3Aim_2)
		{
			FadeOut(t3_dialog2, t3_dialog2ShowingDuration);
			t3_dialog2 = nullptr;

			FadeOut(t3_pressR, t3_pressRShowingDuration);
			t3_pressR = nullptr;

		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Aim)
		{
			FadeOut(clickThis, t3_pressRShowingDuration);
			clickThis = nullptr;
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T2Aim)
		{
			FadeOut(clickThis, t3_pressRShowingDuration);
			clickThis = nullptr;
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T3Aim)
		{
			FadeOut(clickThis, t3_pressRShowingDuration);
			clickThis = nullptr;
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T4Aim)
		{
			FadeOut(t3_dialog2, t3_pressRShowingDuration);
			t3_dialog2 = nullptr;
			FadeOut(SButton, t3_pressRShowingDuration);
			SButton = nullptr;

			isMoveCancled = false;
		}


		if (!isMoveCancled)
		{
			if (ownerbt->GetTutorialState() != ETutorialPhase::S1End)
			{
				ownerbt->NextTutorialState();

			}
		}
		else
		{
			ownerbt->PrevTutorialState();
		}
	}
	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		if (ownerbt->GetTutorialState() == ETutorialPhase::S1T3Aim_1)
		{

			elapsedTime += Time::GetDeltaTime();
			if (elapsedTime > waitTime)
			{
				owner->SwapWeaponType();

				if (owner->currentWeapon == WeaponType::Crossbow)
				{
					if (owner->attackType == AttackType::Normal)
					{
						owner->attackTarget = nullptr;
						owner->phase = PlayerPhase::Aim;
					}
					ownerbt->NextTutorialState();
					elapsedTime = 0.0f;
					t3_dialog2 = owner->AddUIObject<Dialog>("t3_dialog2");
					//t3_dialog2->spriteRenderer->SetImage("PopupUI");
					t3_dialog2->spriteRenderer->useNineSliced = true;
					t3_dialog2->spriteRenderer->nineSlicedImage = ResourceManager::GetNineSlicedImage("Tutorial/MessageBackground");

					t3_dialog2->SetText(L"석궁은 악마를 한 칸 밀어냅니다.\n 악마끼리 부딪히면, 둘 다 피해를 입습니다.", 32);
					t3_dialog2->transform->SetPositionY(150.0f);


					t3_dialog2->textObject->GetComponent<TextRenderer>()->color.A = 0.0f;
					t3_dialog2->spriteRenderer->color.A = 0.0f;

					auto temp_t3_dialog2 = t3_dialog2;
					owner->AddRealTimeTimerFunction([=](bool lu)
													{
														temp_t3_dialog2->textObject->GetComponent<TextRenderer>()->color.A += Time::GetDeltaTime() / t3_pressRShowingDuration;
														temp_t3_dialog2->spriteRenderer->color.A += Time::GetDeltaTime() / t3_pressRShowingDuration;
														if (lu)
														{
															temp_t3_dialog2->textObject->GetComponent<TextRenderer>()->color.A = 1.0f;
															temp_t3_dialog2->spriteRenderer->color.A = 1.0f;
														}
													}, true, t3_pressRShowingDuration, 1);
				}
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1T3Aim_2)
		{
			elapsedTime += Time::GetDeltaTime();
			if (elapsedTime > waitTime)
			{
				return CancleMove(deltaTime);
			}

		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S1End)
		{
			return PlayerAim::OnBehave(deltaTime);
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T1Aim)
		{
			GunPowder* target = dynamic_cast<GunPowder*>(owner->attackTarget);
			if (!target)
			{
				owner->phase = PlayerPhase::Aim;
				owner->attackTarget = nullptr;
			}
			return PlayerAim::OnBehave(deltaTime);
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T2Aim)
		{

			if(Input::IsDown(KeyCode::MouseLeft))
			{
				FadeOut(t3_dialog2, 0.4f);
				t3_dialog2 = nullptr;
				
			}

			if (owner->GetOwner()->GetObjectByName<HighlightManager>("HighlightManager")->tutorial_target ==
				owner->attackTarget)
			{
				return PlayerAim::OnBehave(deltaTime);
			}
			else
			{
				owner->phase = PlayerPhase::Aim;
				owner->attackTarget = nullptr;
				return PlayerAim::OnBehave(deltaTime);
			}
		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T3Aim)
		{
			Chandelier* target = dynamic_cast<Chandelier*>(owner->attackTarget);
			if (target)
			{
				return PlayerAim::OnBehave(deltaTime);
			}
			else
			{
				owner->phase = PlayerPhase::Aim;
				owner->attackTarget = nullptr;
				return PlayerAim::OnBehave(deltaTime);
			}

		}
		else if (ownerbt->GetTutorialState() == ETutorialPhase::S2T4Aim)
		{
			if (Input::IsDown(KeyCode::S) || Input::IsDown(KeyCode::ArrowDown))
			{
				return PlayerAim::OnBehave(deltaTime);
			}
		}
		else
		{
			elapsedTime += Time::GetDeltaTime();
			if (elapsedTime > waitTime)
			{
				return PlayerAim::OnBehave(deltaTime);
			}
		}


		return BT::Status::Running;
	}
	PlayeTutorialrState* ownerbt;
	float waitTime;
	float elapsedTime;


	Object* clickThis;
	Object* t3_pressR;
	Dialog* t3_dialog2;
	Object* SButton;

	float t1_clickThisShowingDuration = 0.4f;
	float t3_pressRShowingDuration = 0.4f;
	float t3_dialog2_showedDuration = 1.5f;
	float t3_dialog2ShowingDuration = 0.4f;
	float t3_block_player_action_time = 1.0f;
	float t3_block_player_action_elapsed_time = 0.0f;
};


class PlayerTutorialWait : public PlayerWait
{
#define PlayerTutorialWaitReflect(x)
	GENERATED_BODY(PlayerTutorialWait, PlayerAim)
public:
	virtual void OnEnter() override
	{
		PlayerWait::OnEnter();
		ownerbt = GetBehaviourTree<PlayeTutorialrState>();
	}
	virtual void OnExit() override
	{
		PlayerWait::OnExit();
	}
	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		if (owner->gm->GetCurrentPhase() == Phase::Player_Action)
		{
			return BT::Status::Success;
		}
		else
		{
			return BT::Status::Running;
		}
	}
	PlayeTutorialrState* ownerbt;
};
