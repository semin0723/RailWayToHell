#pragma once

#include "Engine.h"

struct IOnDie
{
	virtual void operator()() = 0;
};
struct IOnHit
{
	virtual void operator()(int damage) = 0;
};
class HealthComponent : public Script
{
	Object* hp_bar{ nullptr };
	SpriteRenderer* hp_bar_renderer{ nullptr };
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void SetUIYPos(float yPos);
	void Init(int health, IOnHit* onHit = nullptr, IOnDie* onDie = nullptr);
	void Hit(int damage);
	void GetDamage(int damage);

	void ResetHp() { health = init_health;  }
	const int GetHp() const { return health; }
	const int GetMaxHp() const { return init_health; }
	bool IsDie() const { return health <= 0; }

	Object* GetHPBarObject();
	void TurnOnFlickering(int decreasedHealth);
	void TurnOffFlickering();
protected:
	void ChangeHPBarSprite(int maxHealth, int curHealth);

	IOnDie* onDie;
	IOnHit* onHit;
	int health;
	int init_health;
	
	bool isFlickering = false;
	float flickering_elapsedTime = 0.0f;
	const float flickering_interval = 0.3f;
	int flickering_health = 0;
	bool flickered = false;

	bool init = false;
};

