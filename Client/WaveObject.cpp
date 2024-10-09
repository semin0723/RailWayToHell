#include "WaveObject.h"
#include "TextObject.h"

void WaveObject::Awake()
{
	spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetImage(ResourceManager::GetImage("meleeEnemy/idle"));
	spriteRenderer->SetSize(100.f, 100.f);

	spawnText = AddUIObject<TextObject>();

	spawnText->transform->SetPositionX(50.f);
	spawnText->transform->SetParent(transform);

}

void WaveObject::Start()
{
}

void WaveObject::Update()
{
}

void WaveObject::SetSpawnCount(int count)
{
	spawnCount = count;
	spawnText->GetComponent<TextRenderer>()->text += std::to_wstring(spawnCount);
}

void WaveObject::SetMonsterImage(std::string key)
{
	spriteRenderer->SetImage(ResourceManager::GetImage(key));
}
