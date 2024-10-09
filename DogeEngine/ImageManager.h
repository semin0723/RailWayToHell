#pragma once
#include <vector>
#include "DirectXFramework.h"
#include "defines.h"

NAMESPACE_DOGEENGINE_BEGIN

class ImageManager
{
public:
	static ComPtr<ID2D1Bitmap> GetD2DBitmapFromFile(std::wstring path, float quality);
	//static ComPtr<ID2D1Bitmap> GetD2DBitmapFromFile(std::wstring path);

	static std::vector<ComPtr<ID2D1Bitmap>> GetD2DBitmapsFromGIFFile(std::wstring path);
};

NAMESPACE_DOGEENGINE_END