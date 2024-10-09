#include "OrderStaticTileAttack.h"
#include "GameManager.h"
#include "Boss.h"
#include "MonsterComponent.h"

OrderStaticTileAttack::~OrderStaticTileAttack()
{
	/*Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->UnRegisterCommandList(action);*/
}

void OrderStaticTileAttack::Initialize()
{
}

BTStatus OrderStaticTileAttack::Behave()
{
	GameObject* leftHand = Parent->GetOwner<Boss>()->leftHand;
	GameObject* rightHand = Parent->GetOwner<Boss>()->rightHand;

	auto [leftTargetIndex, rightTargetIndex] = GetAttackArea();

	if (leftHand != nullptr) {
		leftHand->GetComponent<MonsterComponent>()->SetActType(ActType::BossAttackReady);
		leftHand->GetComponent<MonsterComponent>()->SetTargetLocation(leftTargetIndex);
		leftHand->SetCurrentTileIndex(leftTargetIndex);
	}
	if (rightHand != nullptr) {
		rightHand->GetComponent<MonsterComponent>()->SetActType(ActType::BossAttackReady);
		rightHand->GetComponent<MonsterComponent>()->SetTargetLocation(rightTargetIndex);
		rightHand->SetCurrentTileIndex(rightTargetIndex);
	}
	return CurStatus = BTStatus::Success;
}

std::pair<int, int> OrderStaticTileAttack::GetAttackArea()
{
	int index = Random::Range(0, 2);

	return attackTile[index];
}