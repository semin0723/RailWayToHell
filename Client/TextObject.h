#pragma once
#include "Engine.h"

class TextObject : public Object
{
public:
	TextObject() {}
	~TextObject() {}

	virtual void Awake() override;

private:
	TextRenderer* textRenderer;

};

