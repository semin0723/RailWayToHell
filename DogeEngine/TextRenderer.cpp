#include "TextRenderer.h"
#include "DrawingManager.h"

void TextRenderer::Draw()
{
	DrawingManager::PrintText(this);
}

void TextRenderer::SetAlignment(TextAlignHorizontal horizontal, TextAlignVertical vertical)
{
	alignVertical = vertical;
	alignHorizontal = horizontal;
}

void TextRenderer::SetWeight(TextWeight weight)
{
	this->weight = weight;
}

void TextRenderer::SetWeight(int weight)
{
	if (weight < 0 || weight > 3) return;
	this->weight = (TextWeight)weight;
}

void TextRenderer::SetStyle(TextStyle style)
{
	this->style = style;
}

void TextRenderer::SetFontSize(float size)
{
	fontSize = size;
}

void TextRenderer::SetFont(CustomFont* font)
{
	this->font = font;
}
