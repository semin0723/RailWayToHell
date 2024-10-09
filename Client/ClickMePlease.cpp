#include "ClickMePlease.h"
#include "Player.h"

void ClickMePlease::Awake()
{
	SetDrawingPriority(INT_MIN);

	collider = AddComponent<BoxCollider>();
	collider->SetSize(2000, 1200);

	eventManager = AddComponent<EventManager>();
	eventManager->AddEventListener(EventType::ClickDown, [this]()
		{
			Player* player = GetOwner()->GetObjectByName<Player>("Player");
			if (player == nullptr) return;

			if (player->phase == PlayerPhase::Aim)
			{
				AudioFramework::Play_BankSound("Sound", "UI_Wrongtarget");
			}
		});
}