#include "SpawnWarning.h"

void SpawnWarning::Awake()
{
	spriteRenderer = AddComponent<SpriteRenderer>();
	transform->SetPositionY(-130.f);
	transform->SetPosition(-5.f, -130.f, 0);
}

void SpawnWarning::SetMonsterKey(std::string monsterType)
{
	spriteRenderer->SetImage(ResourceManager::GetImage(key + monsterType));
	spriteRenderer->SetSizeAuto();
}
