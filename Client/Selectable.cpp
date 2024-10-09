#include <functional>
#include "Selectable.h"
#include "Player.h"

void Selectable::Awake()
{
}

void Selectable::Start()
{
	eventManager = GetOwner()->GetComponent<EventManager>();
	if (eventManager == nullptr)
	{
		eventManager = GetOwner()->AddComponent<EventManager>();
	}

	Collider* collider = GetOwner()->GetComponent<Collider>();
	if (collider == nullptr)
	{
		printf("please attach collider to Object ID: %d\n", GetOwner()->GetID());
	}


	// 이제 일반 공격, 넉백 공격은 Player 안에서 분기함.
	eventManager->AddEventListener(EventType::ClickDown, [this]()
		{
			Player* player = GetOwner()->GetOwner()->GetObjectByName<Player>("Player");
			player->SetAttackTarget(GetOwner<GameObject>());
		});

	/*if (GetOwner<GameObject>()->type == EType::Enemy)
	{
		eventManager->AddEventListener(EventType::RightClickDown, [this]()
			{
				Player* player = GetOwner()->GetOwner()->GetObjectByName<Player>("Player");
				player->SetKnockbackTarget(GetOwner<GameObject>());
			});
	}*/
}