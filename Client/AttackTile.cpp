#include "AttackTile.h"
#include "GameManager.h"
#include "Boss.h"
#include "MonsterComponent.h"
#include "SceneEffect.h"

void AttackTile::Initialize()
{
}

BTStatus AttackTile::Behave()
{
	GameManager* gameManager = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager");
	Phase::Type curPhaseType = gameManager->GetCurrentPhase();

	GameObject* leftHand = Parent->GetOwner<Boss>()->leftHand;
	GameObject* rightHand = Parent->GetOwner<Boss>()->rightHand;

	if (Parent->GetOwner<Boss>()->GetAnimationState() == BossAnimationState::Spawn || 
		Parent->GetOwner<Boss>()->GetAnimationState() == BossAnimationState::EyeClose ||
		Parent->GetOwner<Boss>()->GetAnimationState() == BossAnimationState::EyeOpen) {
		return CurStatus = BTStatus::Failure;
	}


	if (curPhaseType == Phase::Type::Decide_Enemy_Action && !isRegistAction) {

		// 1. select attack tile index
		auto [leftAttackTile, rightAttackTile] = GetAttackArea();
		
		// 2. move to tile's potision x and position y = 0
		// 3. delete object in tile because hand object is in the air
		GameObject* leftHand = Parent->GetOwner<Boss>()->leftHand;
		GameObject* rightHand = Parent->GetOwner<Boss>()->rightHand;

		if (leftHand == nullptr && rightHand == nullptr) {
			// 1. Damage to Boss Body
			// 2. Spawn new Hand to 0, 6
			if (Parent->GetOwner<Boss>()->IsEyeOpen() == false) {
				Parent->GetOwner<Boss>()->OpenEye();
				Parent->GetOwner<Boss>()->waitTurn = 0;
			}
			gameManager->UnRegisterCommandList(action);

			return CurStatus = BTStatus::Success;
		}

		float moveInterval = 0.5f;
		
		if (leftHand != nullptr) {
			leftHand->GetComponent<MonsterComponent>()->SetTargetLocation(leftAttackTile);


			float startX = leftHand->transform->GetPosition().x;
			float startY = leftHand->transform->GetPosition().y;
			float targetX = DEFINE::GetTilePositionX(leftAttackTile);
			float t = 0.0f;

			leftHand->AddTimerFunction([=](bool lu) mutable {
				if (!lu) {
					t += Time::GetDeltaTime() / moveInterval;
					leftHand->transform->SetPositionX(Math::EaseInCubic(startX, targetX, t));
					leftHand->transform->SetPositionY(Math::EaseInCubic(startY, 0.f, t));
				}
				else {
					leftHand->transform->SetPosition(targetX, 0.f, 0);
					gameManager->tileObjects[leftHand->GetCurrentTileIndex()]->object = nullptr;
					/*if (!leftSpawn) {
						
					}
					else {
						leftSpawn = false;
					}*/
					printf("tile %d cleaned leftHand\n", leftHand->GetCurrentTileIndex());
					leftHand->SetCurrentTileIndex(leftAttackTile);

				}
			}, true, moveInterval, 1);
			leftHand->GetComponent<MonsterComponent>()->SetActType(ActType::BossAttackReady);
			AudioFramework::Play_BankSound("Sound", "SFX_Boss_Uphand");
		}
		else {
			// 돌진형, 근접 랜덤 생성? 
			printf("SpawnMonster LEFTHand %d\n", Parent->GetOwner<Boss>()->targetPlayer->GetCurrentTileIndex() - 1);
			gameManager->SpawnMonsterInTile(Parent->GetOwner<Boss>()->targetPlayer->GetCurrentTileIndex() - 1, Random::Range(1, 3), 9999);

		}

		if (rightHand != nullptr) {
			rightHand->GetComponent<MonsterComponent>()->SetTargetLocation(rightAttackTile);


			float startX = rightHand->transform->GetPosition().x;
			float startY = rightHand->transform->GetPosition().y;
			float targetX = DEFINE::GetTilePositionX(rightAttackTile);
			float t = 0.0f;

			rightHand->AddTimerFunction([=](bool lu) mutable {
				if (!lu) {
					t += Time::GetDeltaTime() / moveInterval;
					rightHand->transform->SetPositionX(Math::EaseInCubic(startX, targetX, t));
					rightHand->transform->SetPositionY(Math::EaseInCubic(startY, 0.f, t));
				}
				else {
					rightHand->transform->SetPosition(targetX, 0.f, 0);
					gameManager->tileObjects[rightHand->GetCurrentTileIndex()]->object = nullptr;
					/*if (!rightSpawn) {
					}
					else {
						rightSpawn = false;
					}*/
					printf("tile %d cleaned rightHand\n", rightHand->GetCurrentTileIndex());
					rightHand->SetCurrentTileIndex(rightAttackTile);
				}
			}, true, moveInterval, 1);
			rightHand->GetComponent<MonsterComponent>()->SetActType(ActType::BossAttackReady);
			AudioFramework::Play_BankSound("Sound", "SFX_Boss_Uphand");
		}
		else {
			printf("SpawnMonster RightHand %d\n", Parent->GetOwner<Boss>()->targetPlayer->GetCurrentTileIndex() + 1);
			gameManager->SpawnMonsterInTile(Parent->GetOwner<Boss>()->targetPlayer->GetCurrentTileIndex() + 1, Random::Range(1, 3), 9999);
		}

		// 1. attack to targetTile
		// 2. add object in tile
		float actionInterval = 0.2f;
		bool isInit = false;

		auto attackAction = [this, actionInterval, isInit, gameManager]() mutable {
			GameObject* leftHand = Parent->GetOwner<Boss>()->leftHand;
			GameObject* rightHand = Parent->GetOwner<Boss>()->rightHand;

			float t = 0.0f;
			if (!isInit) {
				if (leftHand != nullptr) {
					printf("LeftHand alive\n");
					leftHand->AddTimerFunction([=](bool lu) mutable {
						if (!lu) {
							t += Time::GetDeltaTime() / actionInterval;
							leftHand->transform->SetPositionY(Math::EaseInCubic(0.f, -320.f, t));
						}
						else {
							leftHand->transform->SetPositionY(-320.f);
							// 즉사
							leftHand->GetOwner()->GetObjectByName<GameManager>("GameManager")->
								DamageInTile(leftHand->GetComponent<MonsterComponent>()->GetTargetLocation(), 100);

							AudioFramework::Play_BankSound("Sound", "SFX_Boss_Atk");
							//leftHand->GetOwner()->GetObjectByName<GameManager>("GameManager")->
								//ObjectMove(leftHand->GetCurrentTileIndex(), leftHand->GetComponent<MonsterComponent>()->GetTargetLocation());
							gameManager->tileObjects[leftHand->GetCurrentTileIndex()]->object = leftHand;
							printf("tile %d setted leftHand\n", leftHand->GetCurrentTileIndex());
							leftHand->GetComponent<MonsterComponent>()->SetActType(ActType::BossAttack);



						}

						}, true, actionInterval, 1);
				}

				if (rightHand != nullptr) {
					printf("RightHand alive\n");
					rightHand->AddTimerFunction([=](bool lu) mutable {
						if (!lu) {
							t += Time::GetDeltaTime() / actionInterval;
							rightHand->transform->SetPositionY(Math::EaseInCubic(0.f, -320.f, t));
						}
						else {
							rightHand->transform->SetPositionY(-320.f);
							// 즉사
							rightHand->GetOwner()->GetObjectByName<GameManager>("GameManager")->
								DamageInTile(rightHand->GetComponent<MonsterComponent>()->GetTargetLocation(), 100);

							AudioFramework::Play_BankSound("Sound", "SFX_Boss_Atk");
							//rightHand->GetOwner()->GetObjectByName<GameManager>("GameManager")->
								//ObjectMove(rightHand->GetCurrentTileIndex(), rightHand->GetComponent<MonsterComponent>()->GetTargetLocation());
							gameManager->tileObjects[rightHand->GetCurrentTileIndex()]->object = rightHand;
							printf("tile %d setted rightHand\n", rightHand->GetCurrentTileIndex());
							rightHand->GetComponent<MonsterComponent>()->SetActType(ActType::BossAttack);
						}

						}, true, actionInterval, 1);
				}

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
		isRegistAction = false;
		Parent->GetOwner()->GetComponent<MonsterComponent>()->SetActive(false);
		gameManager->RegisterCommandList(-10000, action);

		return CurStatus = BTStatus::Success;
	}

	return CurStatus = BTStatus::Failure;
}

std::pair<int, int> AttackTile::GetAttackArea()
{
	int index = Random::Range(0, 2);

	return attackTile[index];
}
