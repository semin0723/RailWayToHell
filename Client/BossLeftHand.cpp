#include "BossLeftHand.h"
#include "MonsterComponent.h"
#include "Selectable.h"
#include "Highlightable.h"
#include "PrintActionPriority.h"
#include "Boss.h"
#include "defines.h"

void BossLeftHand::Awake()
{
	SetDrawingPriority(OrderLayer::Enemy_Boss_Hand);

	type = EType::Enemy;
	spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetImage(ResourceManager::GetImage("Boss/FistLeft"));
	spriteRenderer->SetSizeAuto();

	monsterComponent = AddComponent<MonsterComponent>();
	monsterComponent->SetUIYPos(spriteRenderer->GetHeight());
	monsterComponent->hitSoundName = "SFX_Boss_Hurt";

	BoxCollider* collider = AddComponent<BoxCollider>();
	collider->SetSize(spriteRenderer->GetWidth() * 0.75f, spriteRenderer->GetHeight());
	collider->SetAnchor(0.0f, spriteRenderer->GetHeight() * 0.5f);

	AddComponent<Selectable>();
	AddComponent<Highlightable_Boss>();
	AddComponent<PrintActionPriority>();
}

void BossLeftHand::OnDestroy()
{
	body->HandCrashed<BossLeftHand>();
}
