#include "TargetIsNone.h"
#include "GameManager.h"
#include "GameObject.h"
#include "MonsterComponent.h"

TargetIsNone::~TargetIsNone()
{
	if (Child != nullptr) {
		delete Child;
		Child = nullptr;
	}
}

void TargetIsNone::Initialize()
{
}

BTStatus TargetIsNone::Behave()
{
    if (CurStatus == BTStatus::Running) {
        return CurStatus = Child->Behave();
    }

    std::vector<TileObject*>& tileInfo = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->tileObjects;
    MonsterComponent* monsterComponent = Parent->GetOwner()->GetComponent<MonsterComponent>();

    if (tileInfo[monsterComponent->GetTargetLocation()]->object == nullptr) {
        monsterComponent->SetActType(ActType::Move);
        return CurStatus = Child->Behave();
    }
    else {
        return CurStatus = BTStatus::Failure;
    }
}
