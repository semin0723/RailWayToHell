#include "TacticModeBtn.h"
#include "GameManager.h"
#include "Highlightable.h"

void TacticModeBtn::Awake()
{
	renderer = AddComponent<SpriteRenderer>();
	renderer->SetImage("UI/TacticModeBtn/On");
	renderer->SetSize(150, 150);

	collider = AddComponent<BoxCollider>();
	collider->SetSizeAutomatically();
	
	Vector2 pos = Camera::GetScreenPointByRatio(0.9f, 0.1f);
	transform->SetPosition(pos.ToVector3());

	eventManager = AddComponent<EventManager>();
}
void TacticModeBtn::Start()
{
	gameManager = GetOwner()->GetObjectByName<GameManager>("GameManager");

	eventManager->AddEventListener(EventType::ClickDown, [this]()
		{
			if (on == true) TurnOff();
			else if (on == false) TurnOn();
		});
}

void TacticModeBtn::TurnOn()
{
	AudioFramework::Play_BankSound("Sound", "UI_Mouseover");

	on = true;
	renderer->SetImage("UI/TacticModeBtn/On");
}

void TacticModeBtn::TurnOff()
{
	AudioFramework::Play_BankSound("Sound", "UI_Mouseover");

	on = false;
	renderer->SetImage("UI/TacticModeBtn/Off");
}
