#pragma once
#include "DirectXFramework.h"
#include "Structures.h"

struct NineSlicedImage
{
	ComPtr<ID2D1Bitmap> image;
	float leftGap{ 0.0f };
	float rightGap{ 0.0f };
	float topGap{ 0.0f };
	float bottomGap{ 0.0f };
};