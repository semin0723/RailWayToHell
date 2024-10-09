#pragma once
#include "Engine.h"


// ���� ���������� ��� ������Ʈ�� �ε������͸� ǥ���ϴ� ���� ��� ��ư


class TacticModeBtn : public Object
{
	SpriteRenderer* renderer;
	class GameManager* gameManager;
	EventManager* eventManager;
public:
	BoxCollider* collider;
	bool on = true;

	void Awake() override;
	void Start() override;

	void TurnOn();
	void TurnOff();
};

