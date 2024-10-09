#pragma once
#include "Engine.h"

class WaveIcon : public Object
{
public:
	WaveIcon() {}
	~WaveIcon() {}

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

private:
	SpriteRenderer* spriteRenderer;
};

