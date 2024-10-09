#pragma once
#include "Engine.h"

class WeaponStateUI : public Object
{
	class Player* player;
public:
	SpriteRenderer* renderer;

	WeaponStateUI() {}
	~WeaponStateUI() {}

	virtual void Awake() override;
	virtual void Start() override;

	// 이후 플레이어의 무기 상태에 대한 변수가 추가되면 매개변수를 변경할 것.
	void ChangeWeaponState(std::string state);
};

