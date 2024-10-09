#include "TurnObject.h"

void TurnObject::Awake()
{
	spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetImage(ResourceManager::GetImage("UI/CandleFire"));
	spriteRenderer->SetSize(60.f, 100.f);
}

void TurnObject::Start()
{
}

void TurnObject::OnDestroy()
{
}
