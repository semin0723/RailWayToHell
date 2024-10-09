#pragma once
#include "Engine.h"

class TileSelected : public Object
{
public:
	TileSelected() {}
	~TileSelected() {}

	virtual void Awake() override;

private:
	SpriteRenderer* spriteRenderer = nullptr;
};

