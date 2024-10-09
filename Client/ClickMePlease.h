#pragma once
#include "Engine.h"

// 플레이어 에임 모드에서 타겟 외 클릭시 탄 걸리는 소리 나게 하는 놈
// GameManager.Awake()일 때 씬에 추가합니다.

class ClickMePlease : public Object
{
	BoxCollider* collider;
	EventManager* eventManager;
public:
	void Awake() override;
};

