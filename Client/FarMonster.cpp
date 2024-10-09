#include "FarMonster.h"
#include "GameManager.h"
#include "MonsterComponent.h"
#include "FarMonsterAIComponent.h"
#include "Selectable.h"

#include "HealthComponent.h"
#include "Highlightable.h"
#include "PrintActionPriority.h"
#include "Engine.h"

void FarMonster::Awake()
{
	SetDrawingPriority(OrderLayer::Enemy);
	spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/idle"));
	spriteRenderer->SetSizeAuto();

	aiComponent = AddComponent<FarMonsterAIComponent>();

	type = EType::Enemy;


	MonsterComponent* monsterComponent = AddComponent<MonsterComponent>();
	monsterComponent->SetUIYPos(spriteRenderer->GetHeight());
	monsterComponent->attackSoundName = "Monster_Range_Atk";
	monsterComponent->moveSoundName = "Monster_Move";
	monsterComponent->hitSoundName = "Monster_Range_Hurt";
	monsterComponent->deathSoundName = "Monster_Range_Death";


	BoxCollider* collider = AddComponent<BoxCollider>();
	collider->SetSize(spriteRenderer->GetWidth() * 0.75f, spriteRenderer->GetHeight());
	collider->SetAnchor(0.0f, spriteRenderer->GetHeight() * 0.5f);


	AddComponent<Selectable>();
	AddComponent<Highlightable_Monster>();
	AddComponent<PrintActionPriority>();
	
	// idle animation
	animator = AddComponent<Animator>();
	animator->SetAnimation(*ResourceManager::GetAnimation("range/idle"), true);
	SetAnimation("Idle");
}

void FarMonster::Start()
{
	gm = GetOwner()->GetObjectByName<GameManager>("GameManager");
}

void FarMonster::Update()
{
	if (GetComponent<MonsterComponent>()->GetActDirection() == -1) {
		transform->SetScaleX(1);
	}
	else {
		transform->SetScaleX(-1);
	}
	//GetComponent<BoxCollider>()->DrawBounds();
}

void FarMonster::SetAnimation(std::string key)
{
	if (key != "Idle" || key != "Ready")
	{
		animator->enable = false;
	}


	if (key == "Move") {
		spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/move"));
		spriteRenderer->SetSizeAuto();
	}
	else if (key == "Attack") {
		spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/attack"));
		spriteRenderer->SetSizeAuto();
	}
	else if (key == "Idle") {
		//spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/idle"));
		//spriteRenderer->SetSizeAuto();
		animator->SetAnimation(*ResourceManager::GetAnimation("range/idle"), true);
		animator->enable = true;
		spriteRenderer->SetSize(191, 192);
	}
	else if (key == "Ready") {
		//spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/attackReady"));
		animator->SetAnimation(*ResourceManager::GetAnimation("range/attackReady"), true);
		animator->enable = true;
		spriteRenderer->SetSizeAuto();
	}
	else if (key == "Hit") {
		spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/hit"));
		spriteRenderer->SetSizeAuto();

		AddTimerFunction([this](bool lu) mutable {
			MonsterComponent* monsterComponent = GetComponent<MonsterComponent>();
			switch (monsterComponent->GetActType()) {
			case ActType::Idle:
				/*spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/idle"));
				spriteRenderer->SetSizeAuto();*/
				animator->SetAnimation(*ResourceManager::GetAnimation("range/idle"), true);
				animator->enable = true;
				spriteRenderer->SetSize(191, 192);
				break;
			case ActType::Attack:
				spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/attackReady"));
				spriteRenderer->SetSizeAuto();
				break;
			default:
				/*spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/idle"));
				spriteRenderer->SetSizeAuto();*/
				animator->SetAnimation(*ResourceManager::GetAnimation("range/idle"), true);
				animator->enable = true;
				spriteRenderer->SetSize(191, 192);
				break;
			}
		}, false, GetConfig().Monster_HitSpriteDuration, 1);
	}
}
