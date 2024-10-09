#pragma once
#include "../DogeEngine/Component.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include "GameEffect.h"

enum class EnemyType { Boss, Boss_Left, Boss_Right, Rush, Near, Far };
enum class ActType { Idle, Move, Attack, BossAttack, BossAttackReady, BossRest };

class Object;

struct MonsterDie : public IOnDie 
{
	virtual void operator()() override;


	class GameObject* monster;
};

struct MonsterHit : public IOnHit
{
	virtual void operator()(int damage) override;

	
	class GameObject* monster;
};

class MonsterComponent : public HealthComponent
{
public:
	MonsterComponent() {}
	~MonsterComponent() {}

	void SetActDirection	();
	void SetActionOrder		(int order)				{ actionWeight = order; }
	void SetTargetPlayer	(GameObject* player);
	void SetActive			(bool state)			{ active = state; }
	void SetTargetLocation	(int location)			{ targetLocation = location; }
	void SetActType			(ActType type)			{ actType = type; }
	void SetActDirection	(int direction)			{ actDirection = direction; }
	void SetAttackDistance	(int distance)			{ attackDistance = distance; }
	void SetEnemyInfo(int maxHp, int attackDamage, int attackRange, int moveRange, EnemyType enemyType, int actionWeight) {
		this->maxHp			= maxHp;
		this->attackDamage	= attackDamage;
		this->attackRange	= attackRange;
		this->moveRange		= moveRange;
		this->enemyType		= enemyType;
		this->actionWeight	= actionWeight;

		onMonsterDie.monster = GetOwner<GameObject>();
		onMonsterHit.monster = GetOwner<GameObject>();
		HealthComponent::Init(maxHp, &onMonsterHit, &onMonsterDie);
	}


	const int GetActDirection();
	const int GetActionOrder();
	const ActType GetActType();
	const int GetActScore()			const	{ return actScore; }
	const int GetMaxHp()			const	{ return maxHp; }
	const int GetDamage()			const	{ return attackDamage; }
	const int GetAttackRange()		const	{ return attackRange; }
	const int GetMoveRange()		const	{ return moveRange; }
	const int GetTargetLocation()	const	{ return targetLocation; }
	const int GetAttackIDistance()	const	{ return attackDistance; }
	const bool GetActive()			const	{ return active; }
	Object* GetTargetObject()		const	{ return targetPlayer; }
	const EnemyType GetEnemyType()	const	{ return enemyType; }

	const bool IsBossMonster();

	virtual void Start() override;
	virtual void Update() override;

public:
	std::string attackSoundName;
	std::string moveSoundName;
	std::string hitSoundName;
	std::string deathSoundName;

	class Player* player;

private:
	// 기본 몬스터 정보
	int maxHp					= 0;
	int attackDamage			= 0;
	int attackRange				= 0;
	int moveRange				= 0;
	EnemyType enemyType			= EnemyType::Near;

	
	// 내부 ai 설정시 필요한 요소
	int actionWeight			= 0;
	int actScore				= 0;
	int actDirection			= 0;
	int targetLocation			= 0;
	int attackDistance			= 0;
	bool active					= true;
	ActType actType				= ActType::Idle;

	GameObject* targetPlayer	= nullptr;
	MonsterDie onMonsterDie;
	MonsterHit onMonsterHit;

	bool isMouseOver;

};

