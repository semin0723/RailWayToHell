#include "TileSelected.h"

void TileSelected::Awake()
{
	spriteRenderer = AddComponent<SpriteRenderer>();

	spriteRenderer->SetImage(ResourceManager::GetImage("UI/Selected"));
	spriteRenderer->SetSizeAuto();

	transform->SetPositionY(-20.f);
}
