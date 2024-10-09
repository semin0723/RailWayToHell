#pragma once
#include "DirectXFramework.h"
#include "Structures.h"

struct AtlasImage
{
	ComPtr<ID2D1Bitmap> image;
	std::vector<RectF> rects;
};