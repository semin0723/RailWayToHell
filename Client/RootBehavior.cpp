#include "RootBehavior.h"
#include "GameManager.h"
#include "MonsterComponent.h"
#include "Player.h"

RootBehavior::~RootBehavior()
{
	for (auto& node : Childs) {
		if (node != nullptr) {
			delete node;
			node = nullptr;
		}
	}
	Childs.clear();
}

void RootBehavior::Initialize()
{
	MonsterComponent* monsterComponent = Parent->GetOwner()->GetComponent<MonsterComponent>();

	int targetIndex = static_cast<Player*>(monsterComponent->GetTargetObject())->GetCurrentTileIndex();

	//monsterComponent->SetActDirection((targetIndex > (Parent->GetOwner<GameObject>()->GetCurrentTileIndex()) ? 1 : -1));



}

BTStatus RootBehavior::Behave()
{
	if (CurStatus == BTStatus::Running) {
		for (BaseBTNode* child : Childs) {
			if (child->GetCurStatus() == BTStatus::Running) {
				return CurStatus = child->Behave();
			}
		}
	}
	if (Parent->GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager")->GetCurrentPhase() != Phase::Type::Decide_Enemy_Action) return CurStatus;


	for (BaseBTNode* child : Childs) {
		Initialize();
		CurStatus = child->Behave();
		if (CurStatus == BTStatus::Success || CurStatus == BTStatus::Running) {
			return CurStatus;
		}
	}
	return CurStatus;
}
