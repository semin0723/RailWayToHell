#include "TargetIsMonster.h"
#include "MonsterComponent.h"
#include "Engine.h"
#include "GameManager.h"
#include "GameObject.h"

TargetIsMonster::~TargetIsMonster()
{
	if (Child != nullptr) {
		delete Child;
		Child = nullptr;
	}
}

void TargetIsMonster::Initialize()
{

}

BTStatus TargetIsMonster::Behave()
{
    if (CurStatus == BTStatus::Running) {
        return CurStatus = Child->Behave();
    }

    MonsterComponent* monsterComponent = Parent->GetOwner()->GetComponent<MonsterComponent>();
    TileObject* tileInfo = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->GetTile(monsterComponent->GetTargetLocation());

    if (tileInfo->object != nullptr && tileInfo->object->type == EType::Enemy) {
        monsterComponent->SetActType(ActType::Move);
        return CurStatus = Child->Behave();
    }
    else {
        return CurStatus = BTStatus::Failure;
    }
}