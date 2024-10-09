#include "TestMonster.h"
#include "GameManager.h"
#include "MonsterComponent.h"
#include "NearMonsterAIComponent.h"
#include "Selectable.h"

#include "HealthComponent.h"
#include "Highlightable.h"
#include "PrintActionPriority.h"
#include "Engine.h"

void TestMonster::Awake()
{
	SetDrawingPriority(OrderLayer::Enemy);
	spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/idle"));
	spriteRenderer->SetSizeAuto();

	aiComponent = AddComponent<NearMonsterAIComponent>();

	type = EType::Enemy;

	MonsterComponent* monsterComponent = AddComponent<MonsterComponent>();
	monsterComponent->SetUIYPos(spriteRenderer->GetHeight());
	monsterComponent->attackSoundName = "Monster_Melee_Atk";
	monsterComponent->moveSoundName = "Monster_Move";
	monsterComponent->hitSoundName = "Monster_Melee_Hurt";
	monsterComponent->deathSoundName = "Monster_Melee_Death";

	BoxCollider* collider = AddComponent<BoxCollider>();
	collider->SetSize(spriteRenderer->GetWidth() * 0.75f, spriteRenderer->GetHeight());
	collider->SetAnchor(0.0f, spriteRenderer->GetHeight() * 0.5f);

	AddComponent<Selectable>();
	AddComponent<Highlightable_Monster>();
	AddComponent<PrintActionPriority>();

	// idle animation
	animator = AddComponent<Animator>();
	animator->SetAnimation(*ResourceManager::GetAnimation("melee/idle"), true);
	SetAnimation("Idle");
}

void TestMonster::Start()
{
	gm = GetOwner()->GetObjectByName<GameManager>("GameManager");
}

void TestMonster::Update()
{
	if (GetComponent<MonsterComponent>()->GetActDirection() == -1) {
		transform->SetScaleX(1);
	}
	else {
		transform->SetScaleX(-1);
	}
	//GetComponent<BoxCollider>()->DrawBounds();
}

void TestMonster::SetAnimation(std::string key)
{
	if (key != "Idle" || key != "Ready")
	{
		animator->enable = false;
	}


	if (key == "Move") {
		spriteRenderer->SetImage(ResourceManager::GetImage("meleeEnemy/move"));
		spriteRenderer->SetSizeAuto();
	}
	else if (key == "Attack") {
		spriteRenderer->SetImage(ResourceManager::GetImage("meleeEnemy/attack"));
		spriteRenderer->SetSizeAuto();
	}
	else if (key == "Idle") {
		//spriteRenderer->SetImage(ResourceManager::GetImage("meleeEnemy/idle"));
		//spriteRenderer->SetSizeAuto();
		animator->SetAnimation(*ResourceManager::GetAnimation("melee/idle"), true);
		animator->enable = true;
		spriteRenderer->SetSize(180, 203);
	}
	else if (key == "Ready") {
		//spriteRenderer->SetImage(ResourceManager::GetImage("meleeEnemy/attackReady"));
		animator->SetAnimation(*ResourceManager::GetAnimation("melee/attackReady"), true);
		animator->enable = true;
		spriteRenderer->SetSizeAuto();
	}
	else if (key == "Hit") {
		spriteRenderer->SetImage(ResourceManager::GetImage("meleeEnemy/hit"));

		AddTimerFunction([this](bool lu) mutable {
			MonsterComponent* monsterComponent = GetComponent<MonsterComponent>();
			switch (monsterComponent->GetActType()) {
			case ActType::Idle:
				//spriteRenderer->SetImage(ResourceManager::GetImage("meleeEnemy/idle"));
				animator->SetAnimation(*ResourceManager::GetAnimation("melee/idle"), true);
				animator->enable = true;
				spriteRenderer->SetSize(180, 203);
				break;
			case ActType::Attack:
				spriteRenderer->SetImage(ResourceManager::GetImage("meleeEnemy/attackReady"));
				break;
			default:
				//spriteRenderer->SetImage(ResourceManager::GetImage("meleeEnemy/idle"));
				animator->SetAnimation(*ResourceManager::GetAnimation("melee/idle"), true);
				animator->enable = true;
				spriteRenderer->SetSize(180, 203);
				break;
			}
		}, false, 0.6f, 1);
		
	}
}
