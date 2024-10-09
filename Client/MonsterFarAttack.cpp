#include "MonsterFarAttack.h"
#include "TestMonster.h"
#include "GameManager.h"
#include "MonsterComponent.h"
#include "ICommand.h"
#include "GameEffect.h"

MonsterFarAttack::~MonsterFarAttack()
{
	Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->UnRegisterCommandList(action);
}

void MonsterFarAttack::Initialize()
{
}

BTStatus MonsterFarAttack::Behave()
{
	Phase::Type curPhaseType = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->GetCurrentPhase();

	if (curPhaseType == Phase::Type::Decide_Enemy_Action && !isRegistAction) {

		bool isInit = false;
		float actionInterval = 0.65f;

		auto farAttackAction = [this, isInit, actionInterval]() mutable {
			actionInterval -= Time::GetDeltaTime();


			if (!isInit && actionInterval <= 0.25f) {
				MonsterComponent* monsterComp = Parent->GetOwner()->GetComponent<MonsterComponent>();
				Parent->GetOwner<GameObject>()->SetAnimation("Attack");
				int curPos = Parent->GetOwner<GameObject>()->GetCurrentTileIndex();
				int attackDamage = monsterComp->GetDamage();
				int attackDistance = monsterComp->GetAttackIDistance();
				int attackDir = monsterComp->GetActDirection();

				Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->
					DamageInTile(curPos + (attackDir * attackDistance), attackDamage);


				//int targetIndex = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetTargetLocation();
				//int attackDamage = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetDamage();

				//int curLocation = Parent->GetOwner<GameObject>()->GetCurrentTileIndex();

				//// ���Ÿ� ���Ϳ� �÷��̾ �ڸ��� �ٲ����ν� ���Ÿ� Ÿ�� ��ġ�� ���� ��ġ�ϰ� �ִ� ���� ��.
				//if (curLocation == targetIndex) {
				//	printf("attack tileIndex %d\n", targetIndex + 1);
				//	Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->
				//		DamageInTile(targetIndex + (1 * Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection()), attackDamage);
				//}
				//else if (((targetIndex - curLocation) / (targetIndex - curLocation)) != Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActDirection()) {
				//	printf("attack tileIndex %d\n", targetIndex);
				//	Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->
				//		DamageInTile(targetIndex, attackDamage);
				//}
				//else {
				//	printf("attack tileIndex %d\n", targetIndex);
				//	Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->DamageInTile(targetIndex, attackDamage);
				//}

				if (curPos + (attackDir * attackDistance) >= 0 && curPos + (attackDir * attackDistance) < 7) {
					GameEffect* effect = Parent->GetOwner()->AddObject<GameEffect>();
					effect->CreateEffect(
						"VFX/AcidFire",
						Vector3(
							DEFINE::GetTilePositionX(curPos + (attackDir * attackDistance)),
							Parent->GetOwner()->transform->GetPosition().y + 55.f,
							0
						)
					);
					//effect->GetComponent<SpriteRenderer>()->SetSizeAuto();
					effect->GetComponent<SpriteRenderer>()->SetSize(280, 280);
				}

				AudioFramework::Play_BankSound("Sound", Parent->GetOwner()->GetComponent<MonsterComponent>()->attackSoundName);
				isInit = true;
			}


			if (actionInterval <= 0) isRegistAction = false;
			return actionInterval <= 0;
		};

		action = new FunctionCommand;
		action->doFunction = farAttackAction;

		Parent->GetOwner()->GetComponent<MonsterComponent>()->SetActive(false);
		int actionOrder = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActionOrder();
		Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->RegisterCommandList(actionOrder, action);
		// TODO: ���⼭ ���� �غ� �ִϸ��̼����� ��ȯ �ؾ� ��.
		Parent->GetOwner<GameObject>()->SetAnimation("Ready");

		isRegistAction = true;

		return CurStatus = BTStatus::Success;
	}
	return CurStatus = BTStatus::Failure;
}
