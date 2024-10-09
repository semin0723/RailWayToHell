#pragma once
#include "Engine.h"


class HolyButton : public Object
{
	BoxCollider* collider;
	SpriteRenderer* renderer{ nullptr };
	EventManager* eventManager;
	ComPtr<ID2D1Bitmap> sprite_normal;
	ComPtr<ID2D1Bitmap> sprite_down;

	bool enable = true;
public:
	bool change_sprite_on_hover = false;

	void TurnOffButton();
	void TurnOnButton();

	bool isMouseDownFromHere{ false };
	void Awake() override;
	void Update() override;

	void RegisterSprite(std::string sprite_normal_key, std::string sprite_down_key);

	virtual void OnClicked() {}

	std::function<void()> clicked;
};

