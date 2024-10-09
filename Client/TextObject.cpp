#include "TextObject.h"

void TextObject::Awake()
{
	textRenderer = AddComponent<TextRenderer>();
	textRenderer->SetFont(ResourceManager::GetFont("Font/UI"));
	textRenderer->text = L"x ";
	textRenderer->fontSize = 30.0f;
	textRenderer->color = Color{ 0, 0, 0, 1 };
}
