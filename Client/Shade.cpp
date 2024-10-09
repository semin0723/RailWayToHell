#include "Shade.h"
#include "defines.h"

void Shade::Awake()
{
	renderer = AddComponent<SpriteRenderer>();
	renderer->SetImage("shade");

	renderer->SetSize(Camera::GetMainCamera()->width, Camera::GetMainCamera()->height);
	renderer->color.A = 0.4f;

	SetDrawingPriority(OrderLayer::AimShade);

}