#include "CheckPhase.h"
#include "GameManager.h"
#include "MonsterComponent.h"

CheckPhase::~CheckPhase()
{
	if (Child != nullptr) {
		delete Child;
		Child = nullptr;
	}

}

void CheckPhase::Initialize()
{
}

BTStatus CheckPhase::Behave()
{
	bool monsterActive = Parent->GetOwner()->GetComponent<MonsterComponent>()->GetActive();

	if (monsterActive == true) {
		return CurStatus = BTStatus::Failure;
	}
	else {
		return CurStatus = Child->Behave();
	}
}
