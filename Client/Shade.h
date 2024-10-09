#pragma once
#include "Engine.h"

class Shade : public Object
{
	SpriteRenderer* renderer;
public:
	void Awake() override;
};

