#pragma once
#include <string>
#include "DirectXFramework.h"

struct CustomFont
{
	ComPtr<IDWriteFontSet> fontSet = nullptr;
	ComPtr<IDWriteFontCollection1> fontCollection = nullptr;
	std::wstring fontName;
};