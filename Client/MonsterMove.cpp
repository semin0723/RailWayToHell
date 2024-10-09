#include "MonsterMove.h"
#include "TestMonster.h"
#include "Engine.h"
#include "GameManager.h"
#include "MonsterComponent.h"
#include "defines.h"
#include "ICommand.h"

MonsterMove::~MonsterMove()
{
	Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->UnRegisterCommandList(action);
}

void MonsterMove::Initialize()
{
}

BTStatus MonsterMove::Behave()
{
	Phase::Type curPhaseType = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->GetCurrentPhase();

	if (curPhaseType == Phase::Type::Decide_Enemy_Action && !isRegistAction) {
		bool isInit = false;

		float actionInterval = 0.2f;

		auto moveAction = [this, isInit, actionInterval]() mutable {
			if(actionInterval > -1) actionInterval -= Time::GetDeltaTime();

			if (!isInit && actionInterval <= 0) {
				actionInterval = -1;
				Parent->GetOwner<GameObject>()->SetAnimation("Move");
				int curLocation = Parent->GetOwner<GameObject>()->GetCurrentTileIndex();
				int moveRange = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetMoveRange();
				int actDirection = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection();

				bool canMove = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->CanMove(curLocation + (moveRange * actDirection));
				if (canMove == false) {
					if (Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->tileObjects[curLocation + (moveRange * actDirection)]->object->type == EType::Player) {
						Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->
							DamageInTile(curLocation + (moveRange * actDirection), Parent->GetOwner()->GetComponent<MonsterComponent>()->GetDamage());
						Parent->GetOwner<GameObject>()->SetAnimation("Attack");

						return true;
					}
					else {
						return true;
					}
				}
				Parent->GetOwner<GameObject>()->MoveToTile({ .tileIndex = curLocation + (moveRange * actDirection), .isFinish = &isFinish });
				AudioFramework::Play_BankSound("Sound", Parent->GetOwner()->GetComponent<MonsterComponent>()->moveSoundName);
				isInit = true;
			}

			if (isFinish) isRegistAction = false;
			return isFinish;
		};

		action = new FunctionCommand();
		action->doFunction = moveAction;
		isMove = false;
		isFinish = false;
		Parent->GetOwner()->GetComponent<MonsterComponent>()->SetActive(false);
		int actionOrder = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActionOrder();
		Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->RegisterCommandList(actionOrder, action);

		isRegistAction = true;

		return CurStatus = BTStatus::Success;
	}

	return CurStatus = BTStatus::Failure;
}
