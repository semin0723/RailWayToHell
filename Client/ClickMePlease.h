#pragma once
#include "Engine.h"

// �÷��̾� ���� ��忡�� Ÿ�� �� Ŭ���� ź �ɸ��� �Ҹ� ���� �ϴ� ��
// GameManager.Awake()�� �� ���� �߰��մϴ�.

class ClickMePlease : public Object
{
	BoxCollider* collider;
	EventManager* eventManager;
public:
	void Awake() override;
};

