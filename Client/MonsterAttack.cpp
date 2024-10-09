#include "MonsterAttack.h"
#include "GameManager.h"
#include "TestMonster.h"
#include "MonsterComponent.h"
#include "ICommand.h"
#include "Engine.h"

MonsterAttack::~MonsterAttack()
{
	Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->UnRegisterCommandList(action);
}

void MonsterAttack::Initialize()
{
}

BTStatus MonsterAttack::Behave()
{
	Phase::Type curPhaseType = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->GetCurrentPhase();

	if (curPhaseType == Phase::Type::Decide_Enemy_Action && !isRegistAction) {

		bool isInit = false;
		float actionInterval = 0.4f;

		auto attackAction = [this, isInit, actionInterval]() mutable {
			actionInterval -= Time::GetDeltaTime();

			TestMonster* owner = static_cast<TestMonster*>(Parent->GetOwner());
			Animator* animator = owner->GetComponent<Animator>();

			if (!isInit && actionInterval <= 0.2f) {
				int targetIndex = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetTargetLocation();
				int attackDamage = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetDamage();
				int curLocation = Parent->GetOwner<GameObject>()->GetCurrentTileIndex();

				Parent->GetOwner<GameObject>()->SetAnimation("Attack");

				if (curLocation == targetIndex) {
					printf("near attack tileIndex %d\n", targetIndex + 1);
					targetIndex = curLocation + (1 * Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection());
				}
				else if (targetIndex - curLocation !=  Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection()) {
					printf("near attack tileIndex %d\n", curLocation + Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection());
					targetIndex = curLocation + (1 * Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection());
				}
				else {
					printf("near attack tileIndex %d\n", targetIndex);
				}

				Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->DamageInTile(targetIndex, attackDamage);

				if (targetIndex >= 0 && targetIndex < 7) {
					GameEffect* effect = Parent->GetOwner()->AddObject<GameEffect>();
					effect->CreateEffect(
						"VFX/Claw",
						Vector3(
							DEFINE::GetTilePositionX(targetIndex) - (Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection() * 100.f),
							Parent->GetOwner()->transform->GetPosition().y + 100.f,
							0
						)
					);
					effect->GetComponent<SpriteRenderer>()->SetSizeAuto();
				}

				AudioFramework::Play_BankSound("Sound", Parent->GetOwner()->GetComponent<MonsterComponent>()->attackSoundName);

				isInit = true;
			}

			if (actionInterval <= 0) isRegistAction = false;
			return actionInterval <= 0;
			};

		action = new FunctionCommand;
		action->doFunction = attackAction;
		Parent->GetOwner<GameObject>()->SetAnimation("Ready");
		Parent->GetOwner()->GetComponent<MonsterComponent>()->SetActive(false);
		int actionOrder = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActionOrder();
		Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->RegisterCommandList(actionOrder, action);

		isRegistAction = true;

		return CurStatus = BTStatus::Success;
	}

	return CurStatus = BTStatus::Failure;
}
