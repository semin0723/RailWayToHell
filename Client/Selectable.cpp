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


	// ���� �Ϲ� ����, �˹� ������ Player �ȿ��� �б���.
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