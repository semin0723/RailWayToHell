#pragma once
#include "GameObject.h"
#include "HealthComponent.h"


struct TrapHit : public IOnHit
{
	virtual void operator()(int damage) override;
	class Trap* owner;
};


class Trap : public GameObject
{
public:
	bool init = false;

	virtual void Awake() override;
	virtual void Start() override;
	void OnClicked();
	virtual bool InvokeAttack() = 0;

	std::string hitSoundName;
	std::string attackSoundName;

	bool isAttackSucces;
	int attackDamage;
protected:
	EventManager* eventManager{ nullptr };
	TrapHit trapdie;

};

class Chandelier : public Trap
{
	virtual void Awake() override;
	virtual bool InvokeAttack() override;

	float elapsedtime;
	float moveTime = 0.4f;
	bool isMove;
	bool isFall{ false };
};

class GunPowder : public Trap
{
	float elapsedTime;
	bool isInit;
	float time_gap_between_hit_and_knockback = 0.7f;

	GameObject* left_victim{ nullptr };
	GameObject* right_victim{ nullptr };
public:
	virtual void Awake() override;
	virtual bool InvokeAttack()  override;

	void CreateEffect();
	void Hit(int index);
	void Knockback(GameObject* victim, int dir);
};