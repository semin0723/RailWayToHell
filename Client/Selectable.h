#pragma once
#include "Engine.h"
// ��ݽ� ���� ������ ���� ������Ʈ


class Selectable : public Script
{
	EventManager* eventManager{ nullptr };
public:
	void Awake() override;
	void Start() override;
};

