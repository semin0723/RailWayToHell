#include "Tile.h"



void TileObject::Awake()
{
	tileSpriteObject = AddObject<Object>();
	SpriteRenderer* sprite = tileSpriteObject->AddComponent<SpriteRenderer>();
	sprite->SetImage(ResourceManager::GetImage("UI/Tile"));
	sprite->SetSizeAuto();
	sprite->anchorType = AnchorType::TopCenter;
	sprite->SetHeight(20);
	sprite->color.A = 0.5f;
	sprite->color.G = 0.5f;
	sprite->color.B = 0.5f;

	tileSpriteObject->SetDrawingPriority(OrderLayer::DefaultObject);
	tileSpriteObject->transform->SetPositionY(10.0f);
	tileSpriteObject->transform->SetParent(transform);
}

void TileObject::OnDestroy()
{
	tileSpriteObject->Kill();
}

void TileObject::Update()
{

}

void TileObject::SetActiveTileSprite(bool isActive)
{
	tileSpriteObject->SetEnable(isActive);
}
