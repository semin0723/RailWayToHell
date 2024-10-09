#include "IsEnemyRightInFront.h"
#include "MonsterComponent.h"
#include "GameManager.h"
#include "GameObject.h"

IsEnemyRightInFront::~IsEnemyRightInFront()
{
    if (Child != nullptr) {
        delete Child;
        Child = nullptr;
    }

}

void IsEnemyRightInFront::Initialize()
{

}

BTStatus IsEnemyRightInFront::Behave()
{
    if (CurStatus == BTStatus::Running) {
        return CurStatus = Child->Behave();
    }

    MonsterComponent* monsterComponent = Parent->GetOwner()->GetComponent<MonsterComponent>();

    int targetLocation = Parent->GetOwner<GameObject>()->GetCurrentTileIndex() + monsterComponent->GetActDirection();

    TileObject* tileInfo = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->GetTile(targetLocation);
    if (tileInfo && tileInfo->object == nullptr) {
        return CurStatus = BTStatus::Failure;
    }
    else {
        if (tileInfo && monsterComponent->GetEnemyType() != EnemyType::Far && tileInfo->object->type == EType::Enemy) {
            monsterComponent->SetActType(ActType::Idle);
            return CurStatus = Child->Behave();
        }
        else {
            return CurStatus = BTStatus::Failure;
        }
    }
}
