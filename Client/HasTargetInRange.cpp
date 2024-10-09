#include "HasTargetInRange.h"
#include "MonsterComponent.h"
#include <vector>   
#include "Tile.h"
#include "GameManager.h"
#include "GameObject.h"
#include "Player.h"

HasTargetInRange::~HasTargetInRange()
{
    for (auto& node : Childs) {
        if (node != nullptr) {
            delete node;
            node = nullptr;
        }
    }
    Childs.clear();
}

void HasTargetInRange::Initialize()
{
    // TODO: ���⼭ �� ������ �޾ƿ;� ��.
    std::vector<TileObject*>& tileInfo = Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->tileObjects;

    // TODO: ���� ��ġ���� Ư�� �������� Ž��, ó������ ��ģ ������Ʈ�� �÷��̾�� �������� �̵�, ���Ͱų� ��ģ ������Ʈ�� ������ return fail
    MonsterComponent* monsterComponent = Parent->GetOwner()->GetComponent<MonsterComponent>();

    int curLocation = Parent->GetOwner<GameObject>()->GetCurrentTileIndex();
    int actDirection = monsterComponent->GetActDirection();

    Player* player = monsterComponent->player;
    int player_position = player->GetCurrentTileIndex();

    int offset = abs(player_position - curLocation);
    if (offset <= 2) monsterComponent->SetAttackDistance(offset);



    GameObject* searchedObject = nullptr;

    int searchIndex = 0;
    for (int i = 1; i <= monsterComponent->GetAttackRange(); i++) {
        searchIndex = curLocation + (actDirection * i);
        if (searchIndex < 0 || searchIndex >= tileInfo.size()) break;

        if (monsterComponent->GetEnemyType() != EnemyType::Far) {
            if (tileInfo[searchIndex]->object != nullptr) {
                searchedObject = tileInfo[searchIndex]->object;
                break;
            }
        }
        else {
            if (tileInfo[searchIndex]->object != nullptr && tileInfo[searchIndex]->object->type == EType::Player) {
                searchedObject = tileInfo[searchIndex]->object;
                break;
            }
        }
    }

    if (searchIndex == tileInfo.size()) searchIndex--;
    else if (searchIndex == -1) searchIndex = 0;
    monsterComponent->SetTargetLocation(searchIndex);
}

BTStatus HasTargetInRange::Behave()
{
    // �ൿ�� ������.
    if (CurStatus == BTStatus::Running) {
        for (BaseBTNode* child : Childs) {

            // �������� �ൿ�� �� �ϳ�.
            if (child->GetCurStatus() == BTStatus::Running) {
                return CurStatus = child->Behave();
            }
        }
    }
    if (Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->GetCurrentPhase() != Phase::Type::Decide_Enemy_Action) return CurStatus;

    else {
        Initialize();
        for (BaseBTNode* child : Childs) {
            CurStatus = child->Behave();
            if (CurStatus == BTStatus::Success || CurStatus == BTStatus::Running) {
                return CurStatus;
            }
        }
    }

    return CurStatus;
}
