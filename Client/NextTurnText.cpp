#include "NextTurnText.h"

void NextTurnText::Awake()
{
	SetObjectLayer(ObjectLayer::UI);
	textRenderer = AddComponent<TextRenderer>();
	textRenderer->SetFont(ResourceManager::GetFont("Font/UI"));
	textRenderer->text = L"Next Wave";
	textRenderer->fontSize = 50.0f;
	textRenderer->color = Color{ 0, 0, 0, 1 };
	textRenderer->SetFontSize(50.0f);
}
