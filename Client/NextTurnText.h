#pragma once
#include "Engine.h"

class NextTurnText : public Object
{
public:
	NextTurnText() {}
	~NextTurnText() {}

	virtual void Awake() override;

private:
	TextRenderer* textRenderer;
};

