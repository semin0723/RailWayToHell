#pragma once
#include "Engine.h"


// 현재 스테이지의 모든 오브젝트의 인디케이터를 표시하는 전략 모드 버튼


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

