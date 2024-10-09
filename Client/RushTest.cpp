#include "RushTest.h"
#include "RushMonsterAIComponent.h"
#include "MonsterComponent.h"
#include "GameManager.h"
#include "Highlightable.h"

#include "Selectable.h"

#include "HealthComponent.h"
#include "Highlightable.h"
#include "PrintActionPriority.h"

void RushTest::Awake()
{
	SetDrawingPriority(OrderLayer::Enemy);
	spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetImage(ResourceManager::GetImage("rangeEnemy/idle"));
	spriteRenderer->SetSizeAuto();

	aiComponent = AddComponent<RushMonsterAIComponent>();

	type = EType::Enemy;

	MonsterComponent* monsterComponent = AddComponent<MonsterComponent>();
	monsterComponent->SetUIYPos(spriteRenderer->GetHeight());
	monsterComponent->attackSoundName = "Monster_Rush_Atk_Hit";
	monsterComponent->moveSoundName = "Monster_Move";
	monsterComponent->hitSoundName = "Monster_Rush_Hurt";
	monsterComponent->deathSoundName = "Monster_Rush_Death";

	BoxCollider* collider = AddComponent<BoxCollider>();
	collider->SetSize(spriteRenderer->GetWidth() * 0.75f, spriteRenderer->GetHeight());
	collider->SetAnchor(0.0f, spriteRenderer->GetHeight() * 0.5f);
	AddComponent<Selectable>();
	AddComponent<Highlightable_Monster>();
	AddComponent<PrintActionPriority>();

	// idle animation
	animator = AddComponent<Animator>();
	animator->SetAnimation(*ResourceManager::GetAnimation("rush/idle"), true);
	SetAnimation("Idle");
}

void RushTest::Start()
{
	gm = GetOwner()->GetObjectByName<GameManager>("GameManager");
}

void RushTest::Update()
{
	if (GetComponent<MonsterComponent>()->GetActDirection() == -1) {
		transform->SetScaleX(1);
	}
	else {
		transform->SetScaleX(-1);
	}	
	//GetComponent<BoxCollider>()->DrawBounds();
}

void RushTest::SetAnimation(std::string key)
{
	if (key != "Idle" || key != "Ready")
	{
		animator->enable = false;
	}


	if (key == "Move") {
		spriteRenderer->SetImage(ResourceManager::GetImage("rushEnemy/move"));
		spriteRenderer->SetSizeAuto();
	}
	else if (key == "Attack") {
		spriteRenderer->SetImage(ResourceManager::GetImage("rushEnemy/attack"));
		spriteRenderer->SetSizeAuto();
	}
	else if (key == "Idle") {
		//spriteRenderer->SetImage(ResourceManager::GetImage("rushEnemy/idle"));
		//spriteRenderer->SetSizeAuto();
		animator->SetAnimation(*ResourceManager::GetAnimation("rush/idle"), true);
		animator->enable = true;
		spriteRenderer->SetSize(167, 128);
	}
	else if (key == "Ready") {
		//spriteRenderer->SetImage(ResourceManager::GetImage("rushEnemy/attackReady"));
		animator->SetAnimation(*ResourceManager::GetAnimation("rush/attackReady"), true);
		animator->enable = true;
		spriteRenderer->SetSizeAuto();
	}
	else if (key == "Hit") {
		spriteRenderer->SetImage(ResourceManager::GetImage("rushEnemy/hit"));

		AddTimerFunction([this](bool lu) mutable {
			MonsterComponent* monsterComponent = GetComponent<MonsterComponent>();
			switch (monsterComponent->GetActType()) {
			case ActType::Idle:
				//spriteRenderer->SetImage(ResourceManager::GetImage("rushEnemy/idle"));
				animator->SetAnimation(*ResourceManager::GetAnimation("rush/idle"), true);
				animator->enable = true;
				spriteRenderer->SetSize(167, 128);
				break;
			case ActType::Attack:
				spriteRenderer->SetImage(ResourceManager::GetImage("rushEnemy/attackReady"));
				break;
			default:
				//spriteRenderer->SetImage(ResourceManager::GetImage("rushEnemy/idle"));
				animator->SetAnimation(*ResourceManager::GetAnimation("rush/idle"), true);
				animator->enable = true;
				spriteRenderer->SetSize(167, 128);
				break;
			}
			}, false, 0.6f, 1);
	}
}
