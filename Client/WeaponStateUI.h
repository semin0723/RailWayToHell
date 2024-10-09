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

	// ���� �÷��̾��� ���� ���¿� ���� ������ �߰��Ǹ� �Ű������� ������ ��.
	void ChangeWeaponState(std::string state);
};

