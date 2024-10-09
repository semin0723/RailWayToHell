#pragma once
#include "Engine.h"

class Cursor_Aim : public Object
{
	SpriteRenderer* renderer;
	RectF clientRect{};
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void SetGunCursor();
	void SetCrossbowCursor();
};