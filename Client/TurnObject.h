#pragma once
#include "Engine.h"

class TurnObject : public Object
{
public:
	TurnObject() {}
	~TurnObject() {}

	virtual void Awake() override;
	virtual void Start() override;
	virtual void OnDestroy() override;


private:
	SpriteRenderer* spriteRenderer;
};

