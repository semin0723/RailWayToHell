#include "TargetIsPlayer.h"
#include "MonsterComponent.h"
#include "Engine.h"
#include "GameManager.h"
#include "GameObject.h"

TargetIsPlayer::~TargetIsPlayer()
{
    if (Child != nullptr) {
        delete Child;
        Child = nullptr;
    }
}

void TargetIsPlayer::Initialize()
{

}

BTStatus TargetIsPlayer::Behave()
{
    if (CurStatus == BTStatus::Running) {
        return CurStatus = Child->Behave();
    }

    std::vector<TileObject*>& tileInfo = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->tileObjects;
    MonsterComponent* monsterComponent = Parent->GetOwner()->GetComponent<MonsterComponent>();

    if (tileInfo[monsterComponent->GetTargetLocation()]->object != nullptr && tileInfo[monsterComponent->GetTargetLocation()]->object->type == EType::Player) {
        monsterComponent->SetActType(ActType::Attack);
        return CurStatus = Child->Behave();
    }
    else {
        return CurStatus = BTStatus::Failure;
    }
}
