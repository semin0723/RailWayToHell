#pragma once
#include <string>
#include "DirectXFramework.h"
#include "Component.h"
#include "Structures.h"
#include "Vector2.h"
#include "CustomFont.h"

enum class TextAlignVertical { Top, Center, Bottom };
enum class TextAlignHorizontal { Left, Center, Right };
enum class TextWeight { Thin, Normal, Bold, HeavyBold };
enum class TextStyle { Normal, Italic, Oblique };

class DrawingManager;

class TextRenderer : public Component
{
	friend class DrawingManager;

	TextAlignVertical alignVertical = TextAlignVertical::Top;
	TextAlignHorizontal alignHorizontal = TextAlignHorizontal::Left;
	TextWeight weight = TextWeight::Normal;
	TextStyle style = TextStyle::Normal;
	CustomFont* font = nullptr;
public:
	float fontSize = 20.f;
	std::wstring text;
	Color color{ 1, 1, 1, 1 };
	Vector2 anchor{ 0, 0 };
	//bool wordWrapping = false;

	TextRenderer() {}
	void Draw();
	
	void SetAlignment(TextAlignHorizontal horizontal = TextAlignHorizontal::Left, TextAlignVertical vertical = TextAlignVertical::Top);

	void SetWeight(TextWeight weight);
	// 0 to 3
	void SetWeight(int weight);
	void SetStyle(TextStyle style);

	void SetFontSize(float size);

	// nullptr면 윈도우 기본 폰트 콜렉션을 사용함
	void SetFont(CustomFont* font = nullptr);
};

