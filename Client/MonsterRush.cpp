#include "MonsterRush.h"
#include "GameManager.h"
#include "MonsterComponent.h"
#include "TestMonster.h"
#include "Tile.h"
#include "ICommand.h"

MonsterRush::~MonsterRush()
{
	Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->UnRegisterCommandList(action);
}

void MonsterRush::Initialize()
{

}

BTStatus MonsterRush::Behave()
{
	Phase::Type curPhaseType = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->GetCurrentPhase();

	if (curPhaseType == Phase::Type::Decide_Enemy_Action && !isRegistAction) {
		bool isInit = false;
		isFinish = false;
		isRush = false;

		float actionInterval = 0.8f;
		auto rushAction = [this, isInit, actionInterval]() mutable {
			actionInterval -= Time::GetDeltaTime();

			int curLocation = Parent->GetOwner<GameObject>()->GetCurrentTileIndex();
			int nextLocation = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetTargetLocation();
			int direction = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection();

			if (!isInit) {
				DetectObject();
				int curLocation = Parent->GetOwner<GameObject>()->GetCurrentTileIndex();
				int nextLocation = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetTargetLocation();
				int direction = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection();

				if (Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->tileObjects[nextLocation]->object == nullptr) {
					static_cast<GameObject*>(Parent->GetOwner())->MoveToTile({ .tileIndex = nextLocation, .isFinish = &isFinish });
					Parent->GetOwner<GameObject>()->SetAnimation("Move");
				}
				else {
					static_cast<GameObject*>(Parent->GetOwner())->MoveToTile({ .tileIndex = nextLocation, .easing = Math::EaseInCubic, .ignoreObject = true,.isFinish = &isFinish });
					Parent->GetOwner<GameObject>()->SetAnimation("Attack");
					AudioFramework::Play_BankSound("Sound", "Monster_Rush_Atk");
				}

				isInit = true;
			}

			if (isFinish) {
				if (Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->tileObjects[nextLocation]->object != nullptr && nextLocation != curLocation) {
					isFinish = false;
					if (isRush == false) {
						Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->
							DamageInTile(nextLocation, Parent->GetOwner()->GetComponent<MonsterComponent>()->GetDamage());
						isRush = true;
						AudioFramework::Play_BankSound("Sound", Parent->GetOwner()->GetComponent<MonsterComponent>()->attackSoundName);

						GameEffect* effect = Parent->GetOwner()->AddObject<GameEffect>();
						effect->CreateEffect(
							"VFX/Bite",
							Vector3(
								DEFINE::GetTilePositionX(nextLocation),
								Parent->GetOwner()->transform->GetPosition().y + 100.f,
								0
							)
						);
						effect->GetComponent<SpriteRenderer>()->SetSize(280, 280);
					}
					static_cast<GameObject*>(Parent->GetOwner())->MoveToTile({ .tileIndex = nextLocation - direction, .easing = Math::EaseOutCubic,.isFinish = &isFinish });
					Parent->GetOwner<GameObject>()->SetAnimation("Idle");
				}

			}

			if (actionInterval <= 0) isRegistAction = false;
			return actionInterval <= 0;//추후 연출 종료로 변경.
		};
		action = new FunctionCommand;
		action->doFunction = rushAction;


		Parent->GetOwner()->GetComponent<MonsterComponent>()->SetActive(false);
		Parent->GetOwner<GameObject>()->SetAnimation("Ready");
		int actionOrder = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActionOrder();
		Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->RegisterCommandList(actionOrder, action);

		isRegistAction = true;

		return CurStatus = BTStatus::Success;
	}
	return CurStatus = BTStatus::Failure;
}

void MonsterRush::DetectObject()
{
	MonsterComponent* monsterComponent = Parent->GetOwner()->GetComponent<MonsterComponent>();

	int curLocation = Parent->GetOwner<GameObject>()->GetCurrentTileIndex();
	int curDirection = monsterComponent->GetActDirection();

	std::vector<TileObject*>& tileInfo = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->tileObjects;
	
	int cur = curLocation + curDirection;
	for (; cur >= 0 && cur < tileInfo.size(); cur += curDirection) {
		if (tileInfo[cur]->object != nullptr) break;
	}

	if (cur < 0) cur = 0;
	else if (cur == tileInfo.size()) cur--;

	monsterComponent->SetTargetLocation(cur);
}
