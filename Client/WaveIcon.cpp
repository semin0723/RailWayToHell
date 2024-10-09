#include "WaveIcon.h"

void WaveIcon::Awake()
{
	spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetImage(ResourceManager::GetImage("UI/WaveInfo"));
	spriteRenderer->SetSize(60.f, 60.f);
}

void WaveIcon::Start()
{
}

void WaveIcon::Update()
{
}
