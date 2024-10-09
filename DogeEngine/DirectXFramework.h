#pragma once
#include <exception>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1effects.h>
#include <d2d1effecthelpers.h>
#include <d3d11.h>
#include <dwrite.h> // DirectWrite
#include <dwrite_3.h>
#include <wincodec.h> // WIC
#include <wrl/client.h> // ComPtr
using Microsoft::WRL::ComPtr;

// 웬만하면 ComPtr를 쓸 것.
template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != nullptr)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = nullptr;
	}
}

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}