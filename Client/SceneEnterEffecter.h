#pragma once
#include "Engine.h"

class SceneEnterEffecter : public Object
{
	SpriteRenderer* shader = nullptr;
public:
	void Awake() override;
	void Start() override;
};

