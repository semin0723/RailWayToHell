#include "MonsterComponent.h"
#include "../DogeEngine/Object.h"
#include "Player.h"	
#include "GameManager.h"

void MonsterDie::operator()()
{
	monster->GetOwner()->GetObjectByName<GameManager>("GameManager")->IncreseKillCount();

	if (monster->GetComponent<MonsterComponent>()->GetEnemyType() == EnemyType::Boss)
	{
		AudioFramework::Play_BankSound("Sound", monster->GetComponent<class MonsterComponent>()->deathSoundName);

		GameManager* gameManager = monster->GetOwner()->GetObjectByName<GameManager>("GameManager");

		// UnRegisterGameManager()���� tileObject[i].object�� nullptr�� �ٲ������ �������� �׿���� ��...
		if (static_cast<GameObject*>(gameManager->stageLockObject) == monster)
		{
			gameManager->stageLockObject = nullptr;
		}
		gameManager->monsterObjects.erase(std::remove(gameManager->monsterObjects.begin(), gameManager->monsterObjects.end(), monster), gameManager->monsterObjects.end());
		gameManager->bossMonster = nullptr;

		monster->Kill();
		gameManager->KillThemAll();
	}
	else
	{
		monster->UnRegisterGameManager();
		AudioFramework::Play_BankSound("Sound", monster->GetComponent<class MonsterComponent>()->deathSoundName);
	}
}




void MonsterComponent::SetTargetPlayer(GameObject* player)
{
	targetPlayer = player;
	
	int playerLocation = static_cast<Player*>(targetPlayer)->GetCurrentTileIndex();
	int enemyLocation = GetOwner<GameObject>()->GetCurrentTileIndex();

	actDirection = playerLocation < enemyLocation ? -1 : 1;
}

void MonsterComponent::SetActDirection()
{
	int playerLocation = static_cast<Player*>(targetPlayer)->GetCurrentTileIndex();
	int enemyLocation = GetOwner<GameObject>()->GetCurrentTileIndex();

	actDirection = playerLocation < enemyLocation ? -1 : 1;
}

const int MonsterComponent::GetActDirection()
{
	return actDirection;
}

const int MonsterComponent::GetActionOrder()
{
	int actionOrder = (int)enemyType * 10000 + actionWeight;

	return actionOrder;
}

const ActType MonsterComponent::GetActType()
{
	return actType;
}

const bool MonsterComponent::IsBossMonster()
{
	if (enemyType == EnemyType::Boss) return true;
	if (enemyType == EnemyType::Boss_Left) return true;
	if (enemyType == EnemyType::Boss_Right) return true;
	return false;
}

void MonsterComponent::Start()
{
	HealthComponent::Start();
	if (SpriteRenderer* sprite = GetOwner()->GetComponent<SpriteRenderer>(); sprite) 
	{
		sprite->anchorType = AnchorType::BottomCenter;
	}

	onMonsterDie.monster = GetOwner<GameObject>();
	onMonsterHit.monster = GetOwner<GameObject>();

	player = GetOwner()->GetOwner()->GetObjectByName<Player>("Player");
}

void MonsterComponent::Update()
{
	// ���콺 ȣ���� ���� �Ҹ��� ���� ����� ����?
	// �ܾʉµ�?
	/*if (GetOwner()->GetComponent<Collider>()->IsCollideWithCursor())
	{
		if (!isMouseOver)
		{
			isMouseOver = true;
			AudioFramework::Play_BankSound("Sound", "UI_Mouseover");
		}
	}
	else
	{
		isMouseOver = false;
	}*/
	
	HealthComponent::Update();
}

void MonsterHit::operator()(int damage)
{
	monster->SetAnimation("Hit");

	if (monster->GetComponent<HealthComponent>()->GetHp() > damage)
	{
		GameEffect* hitEffect = monster->AddObject<GameEffect>();
		int randomNum = Random::Range(1, 3);
		hitEffect->CreateEffect("VFX/HitBlood" + std::to_string(randomNum), monster->transform->GetPosition() + Vector3(0, 100.f, 0));
		hitEffect->GetComponent<SpriteRenderer>()->SetSize(100.f, 100.f);
		hitEffect->SetDrawingPriority(OrderLayer::monster_hitEffect);
	}
	else
	{
		GameEffect* hitEffect = monster->AddObject<GameEffect>();
		hitEffect->CreateEffect("VFX/DeadHit", monster->transform->GetPosition() + Vector3(0, 100.f, 0));
		//hitEffect->GetComponent<SpriteRenderer>()->SetSize(100.f, 100.f);
		hitEffect->GetComponent<SpriteRenderer>()->SetSizeAuto();
		hitEffect->SetDrawingPriority(OrderLayer::monster_hitEffect);
	}

	monster->GetComponent<HealthComponent>()->GetDamage(damage);

	AudioFramework::Play_BankSound("Sound", monster->GetComponent<MonsterComponent>()->hitSoundName); // <- ���� ���� �� ���� �Ҹ��� ����. �� �ڵ�� �ƹ� ���۵� ���� �ʴ´�.

	// ������ ��� SetAnimation("Hit") �ȿ��� �ð� ��� �� Idle �Ǵ� AttackReady ��������Ʈ�� ������
	/*monster->AddTimerFunction([=](bool lu)
		{
			monster->SetAnimation("Idle");
		}, false, 0.1f, 1);*/
}
