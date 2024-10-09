#include "SetPattern.h"
#include "GameManager.h"
#include "MonsterComponent.h"
#include "Boss.h"

void SetPattern::Initialize()
{
}

BTStatus SetPattern::Behave()
{
	GameManager* gameManager = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager");
	Phase::Type curPhaseType = gameManager->GetCurrentPhase();

	if (curPhaseType != Phase::Type::Decide_Enemy_Action) {



		return CurStatus = BTStatus::Failure;
	}
	else {
		if (Parent->GetOwner<Boss>()->IsHandAlive() == false &&
			Parent->GetOwner<Boss>()->IsEyeOpen() == false &&
			Parent->GetOwner<Boss>()->GetAnimationState() == BossAnimationState::Idle
			) {
			Parent->GetOwner<Boss>()->OpenEye();
			Parent->GetOwner<Boss>()->waitTurn = 0;
		}
	}

	if (Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActive() == false) {
		return CurStatus = BTStatus::Failure;
	}

	for (int i = 0; i < Childs.size(); i++) {
		if (Childs[i]->GetCurStatus() == BTStatus::Running) {
			printf("Running \n");
			return CurStatus = Childs[i]->Behave();
		}
	}

	if (Parent->GetOwner()->GetComponent<MonsterComponent>()->GetHp() != curBossHp) {
		actionCount = 0;
		curBossHp = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetHp();
	}

	if (curBossHp == 3) {
		// 0번은 고정 타일 공격
		return CurStatus = Childs[0]->Behave();
	}
	else if (curBossHp == 2) {
		if (actionCount == 2) {
			actionCount = 0;
			return CurStatus = Childs[0]->Behave();
		}
		else {
			actionCount++;
			return CurStatus = Childs[1]->Behave();
		}
	}
	else if (curBossHp == 1) {
		printf("actionCount : %d\n", actionCount);
		if (actionCount == 1) {
			actionCount = 0;
			return CurStatus = Childs[0]->Behave();
		}
		else {
			actionCount++;
			return CurStatus = Childs[1]->Behave();
		}
	}

	return CurStatus = BTStatus::Failure;
}
