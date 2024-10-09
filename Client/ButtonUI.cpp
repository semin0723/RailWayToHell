#include "ButtonUI.h"

void HolyButton::TurnOffButton()
{
	enable = false;
	renderer->color = Color{ 0.4f, 0.4f, 0.4f, 1.0f };
}

void HolyButton::TurnOnButton()
{
	enable = true;
	renderer->color = Color{ 1.0f, 1.0f, 1.0f, 1.0f };
}

void HolyButton::Awake()
{
	SetObjectLayer(ObjectLayer::UI);
	SetDrawingPriority(OrderLayer::pause_menu_btn);

	collider = AddComponent<BoxCollider>();
	eventManager = AddComponent<EventManager>();
	renderer = AddComponent<SpriteRenderer>();

	eventManager->AddEventListener(EventType::ClickUp, [this]()
		{
			if (this->isMouseDownFromHere == false) return;
			if (this->enable == false) return;

			clicked();
			OnClicked();
		});
}

void HolyButton::Update()
{
	if (this->enable == false) return;

	// very very very very kludge code
	if (change_sprite_on_hover)
	{
		if (collider->IsCollideWithCursor())
		{
			renderer->SetImage(sprite_down);

			if (Input::IsDown(KeyCode::MouseLeft))
			{
				isMouseDownFromHere = true;
			}
		}
		else
		{
			isMouseDownFromHere = false;
			renderer->SetImage(sprite_normal);
		}
	}
	else
	{
		if (collider->IsCollideWithCursor() && Input::IsDown(KeyCode::MouseLeft))
		{
			isMouseDownFromHere = true;
			renderer->SetImage(sprite_down);
		}
		else if (isMouseDownFromHere == true && collider->IsCollideWithCursor() && Input::IsPress(KeyCode::MouseLeft))
		{
			renderer->SetImage(sprite_down);
		}
		else if (isMouseDownFromHere == true && collider->IsCollideWithCursor() && Input::IsUp(KeyCode::MouseLeft))
		{
			renderer->SetImage(sprite_normal);
		}
		else
		{
			isMouseDownFromHere = false;
			renderer->SetImage(sprite_normal);
		}
	}
}

void HolyButton::RegisterSprite(std::string sprite_normal_key, std::string sprite_down_key)
{
	sprite_normal = ResourceManager::GetImage(sprite_normal_key);
	renderer->SetImage(sprite_normal, true);
	collider->SetSizeAutomatically();
	sprite_down = ResourceManager::GetImage(sprite_down_key);
}
