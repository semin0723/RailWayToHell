#pragma once
// �̰� ��?
#include "Engine.h"

class ShootChandelier : public ActionNode
{
public:
	void Initialize() override;
	BTStatus Behave() override;
};

