#pragma once
#include "Engine.h"
// 사격시 선택 가능한 게임 오브젝트


class Selectable : public Script
{
	EventManager* eventManager{ nullptr };
public:
	void Awake() override;
	void Start() override;
};

