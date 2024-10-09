#include "HealthComponent.h"
#include "GameObject.h"
#include "MonsterComponent.h"

void HealthComponent::Awake()
{
	EType type = GetOwner<GameObject>()->type;
	if (type == EType::Player || type == EType::Enemy)
	{
		hp_bar = GetOwner()->AddObject<Object>();
		hp_bar_renderer = hp_bar->AddComponent<SpriteRenderer>();
	}
}

void HealthComponent::Start()
{
	if (hp_bar != nullptr)
	{
		hp_bar->transform->SetParent(GetOwner()->transform);

		if ((GetOwner()->GetComponent<MonsterComponent>() != nullptr && GetOwner()->GetComponent<MonsterComponent>()->GetEnemyType() == EnemyType::Boss)) {
			hp_bar->transform->SetPositionY(GetOwner()->GetComponent<SpriteRenderer>()->GetHeight() + 150.f);
		}
		else {
			hp_bar->transform->SetPositionY(GetOwner()->GetComponent<SpriteRenderer>()->GetHeight());
		}
		hp_bar->SetDrawingPriority(OrderLayer::hp_bar);
	}
}

void HealthComponent::Update()
{
	EType type = GetOwner<GameObject>()->type;
	if (type == EType::Player || type == EType::Enemy)
	{
		if (isFlickering)
		{
			flickering_elapsedTime += Time::GetDeltaTime();
			if (flickering_elapsedTime >= flickering_interval)
			{
				flickering_elapsedTime -= flickering_interval;
				
				flickered = flickered == true ? false : true;
			}

			if (flickered == true)
			{
				ChangeHPBarSprite(init_health, flickering_health);
			}
			else
			{
				ChangeHPBarSprite(init_health, health);
			}
		}
		else
		{
			ChangeHPBarSprite(init_health, health);
		}
	}
}

void HealthComponent::SetUIYPos(float yPos)
{
	//hp_bar->transform->SetPositionY(yPos);
}

void HealthComponent::Init(int health, IOnHit* onHit, IOnDie* onDie)
{
	this->init_health = health;

	this->health = health;
	this->onHit = onHit;
	this->onDie = onDie;

	init = true;
}

void HealthComponent::Hit(int damage)
{
	// OnHit -> 애니메이션 재생 -> 체력 깎기 -> 0되면 OnDie 호출

	if (onHit)
	{
		(*onHit)(damage);
	}
}

void HealthComponent::GetDamage(int damage)
{
	health -= damage;
	if (health <= 0 && onDie)
	{
		(*onDie)();
	}
}

Object* HealthComponent::GetHPBarObject()
{
	return hp_bar;
}

void HealthComponent::TurnOnFlickering(int decreasedHealth)
{
	flickering_health = health - decreasedHealth;
	isFlickering = true;
}

void HealthComponent::TurnOffFlickering()
{
	isFlickering = false;
	flickered = false;
	flickering_elapsedTime = 0.0f;
}

void HealthComponent::ChangeHPBarSprite(int maxHealth, int curHealth)
{
	hp_bar->transform->SetScaleX(GetOwner()->transform->GetScale().x);

	if (curHealth < 0) curHealth = 0;

	std::string imageKey;

	if (GetOwner()->GetComponent<MonsterComponent>() != nullptr && GetOwner()->GetComponent<MonsterComponent>()->GetEnemyType() == EnemyType::Boss) {
		imageKey = "UI/BossHpBar" + std::to_string(init_health) + "_" + std::to_string(curHealth);
	}
	else {
		imageKey = "UI/HpBar" + std::to_string(init_health) + "_" + std::to_string(curHealth);
	}
	
	hp_bar_renderer->SetImage(ResourceManager::GetImage(imageKey));
	hp_bar_renderer->SetSizeAuto();
}
