#include "AttackToPlayer.h"
#include "GameManager.h"
#include "GameObject.h"
#include "Boss.h"
#include "MonsterComponent.h"
#include "SceneEffect.h"

void AttackToPlayer::Initialize()
{
}

BTStatus AttackToPlayer::Behave()
{
	GameManager* gameManager = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager");
	Phase::Type curPhaseType = gameManager->GetCurrentPhase();

	GameObject* leftFist = Parent->GetOwner<Boss>()->leftHand;
	GameObject* rightFist = Parent->GetOwner<Boss>()->rightHand;

	if (Parent->GetOwner<Boss>()->GetAnimationState() == BossAnimationState::Spawn || 
		Parent->GetOwner<Boss>()->GetAnimationState() == BossAnimationState::EyeClose ||
		Parent->GetOwner<Boss>()->GetAnimationState() == BossAnimationState::EyeOpen) {
		return CurStatus = BTStatus::Failure;
	}

	if (Parent->GetOwner<Boss>()->IsHandAlive() == false) {
		// 1. Damage to Boss Body
		// 2. Spawn new Hand to 0, 6

		if (Parent->GetOwner<Boss>()->IsEyeOpen() == false) {
			Parent->GetOwner<Boss>()->OpenEye();
			Parent->GetOwner<Boss>()->waitTurn = 0;

		}
		gameManager->UnRegisterCommandList(action);

		//spawn = true;

		return CurStatus = BTStatus::Failure;
	}

	if (curPhaseType == Phase::Type::Decide_Enemy_Action && !isRegistAction) {

		int type = GetAttackSubject();

		attackObjectType = type;

		if (type == 0) {
			Parent->GetOwner<Boss>()->OpenEye();
			gameManager->UnRegisterCommandList(action);
			return CurStatus = BTStatus::Success;
		}

		GameObject* targetFist = Parent->GetOwner<Boss>()->GetHand(type);
		if (targetFist == nullptr) {
			if (type == 1) {
				printf("attack to player SpawnMonster LEFTHand %d\n", Parent->GetOwner<Boss>()->targetPlayer->GetCurrentTileIndex() - 1);
				gameManager->SpawnMonsterInTile(Parent->GetOwner<Boss>()->targetPlayer->GetCurrentTileIndex() - 1, Random::Range(1, 3), 9999);
			}
			else {
				printf("attack to player SpawnMonster RightHand %d\n", Parent->GetOwner<Boss>()->targetPlayer->GetCurrentTileIndex() + 1);
				gameManager->SpawnMonsterInTile(Parent->GetOwner<Boss>()->targetPlayer->GetCurrentTileIndex() + 1, Random::Range(1, 3), 9999);
			}

		}
		else {
			float initX = targetFist->transform->GetPosition().x;
			float initY = targetFist->transform->GetPosition().y;
			float targetX = DEFINE::GetTilePositionX(targetFist->GetComponent<MonsterComponent>()->GetTargetLocation());
			float t = 0.0f;

			targetFist->GetComponent<MonsterComponent>()->SetActType(ActType::BossAttackReady);

			float moveInterval = 0.4f;
			// 공격 준비를 위해 위로 이동.
			targetFist->AddTimerFunction([=](bool lu) mutable {
				if (!lu) {
					t += Time::GetDeltaTime() / moveInterval;
					targetFist->transform->SetPositionX(Math::EaseInCubic(initX, targetX, t));
					targetFist->transform->SetPositionY(Math::EaseInCubic(initY, 0.f, t));
				}
				else {
					targetFist->transform->SetPosition(targetX, 0.f, 0);

					gameManager->tileObjects[targetFist->GetCurrentTileIndex()]->object = nullptr;
					//if (!spawn) {
					//}
					//else {
					//	spawn = false;
					//}
					printf("tile %d cleaned Hand\n", targetFist->GetCurrentTileIndex());
					targetFist->SetCurrentTileIndex(targetFist->GetComponent<MonsterComponent>()->GetTargetLocation());
				}

				}, true, moveInterval, 1);
		}

		float actionInterval = 0.2f;
		bool isInit = false;

		// 등록할 공격 함수.
		auto attackAction = [this, isInit, actionInterval, gameManager]() mutable {
			float t = 0.0f;
			GameObject* targetFist = Parent->GetOwner<Boss>()->GetHand(attackObjectType);
			if (targetFist == nullptr) {
				return true;
			}

			if (!isInit) {
				targetFist->AddTimerFunction([=](bool lu) mutable {
					if (!lu) {
						t += Time::GetDeltaTime() / actionInterval;
						targetFist->transform->SetPositionY(Math::EaseInCubic(0.f, -320.f, t));
					}
					else {
						targetFist->transform->SetPositionY(-320.f);
						// 즉사
						targetFist->GetOwner()->GetObjectByName<GameManager>("GameManager")->
							DamageInTile(targetFist->GetComponent<MonsterComponent>()->GetTargetLocation(), 100);

						AudioFramework::Play_BankSound("Sound", "SFX_Boss_Atk");
						//targetFist->GetOwner()->GetObjectByName<GameManager>("GameManager")->
						//	ObjectMove(targetFist->GetCurrentTileIndex(), targetFist->GetComponent<MonsterComponent>()->GetTargetLocation());
						gameManager->tileObjects[targetFist->GetCurrentTileIndex()]->object = targetFist;
						printf("tile %d setted Hand\n", targetFist->GetCurrentTileIndex());
						targetFist->GetComponent<MonsterComponent>()->SetActType(ActType::BossAttack);
					}

					}, true, actionInterval, 1);

				isInit = true;
			}
			actionInterval -= Time::GetDeltaTime();
			if (actionInterval <= 0)
			{
				isRegistAction = false;
				SceneEffect* effect = Parent->GetOwner()->GetOwner()->GetObjectByName<SceneEffect>("SceneEffect");
				effect->ShakeCameraVertical(GetConfig().Effect_ShakeTrainMagnitude * 2.5f, 0.3f, 12);
				effect->ShakeCameraHorizontal(GetConfig().Effect_ShakeTrainMagnitude * 2.5f, 0.3f, 12);
			}
			return actionInterval <= 0;
		};
		action = new FunctionCommand;
		action->doFunction = attackAction;
		isRegistAction = true;
		Parent->GetOwner()->GetComponent<MonsterComponent>()->SetActive(false);
		Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->RegisterCommandList(-999999, action);

		return CurStatus = BTStatus::Success;
	}

	return CurStatus = BTStatus::Failure;
}

int AttackToPlayer::GetAttackSubject()
{
	GameObject* leftFist = Parent->GetOwner<Boss>()->leftHand;
	GameObject* rightFist = Parent->GetOwner<Boss>()->rightHand;

	int targetLocation = Parent->GetOwner<Boss>()->targetPlayer->GetCurrentTileIndex();

	if (leftFist == nullptr && rightFist == nullptr) {
		return 0;
	}

	if (targetLocation >= 0 && targetLocation <= 2) {
		if (leftFist != nullptr) leftFist->GetComponent<MonsterComponent>()->SetTargetLocation(targetLocation);
		return 1;
	}
	else {
		if(rightFist != nullptr) rightFist->GetComponent<MonsterComponent>()->SetTargetLocation(targetLocation);
		return 2;
	}


}
