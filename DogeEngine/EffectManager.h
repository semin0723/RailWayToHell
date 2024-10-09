#pragma once
#include "DirectXFramework.h"
#include "defines.h"
#include "Structures.h"

NAMESPACE_DOGEENGINE_BEGIN

class EffectManager
{
public:
	static ComPtr<ID2D1Effect> GetColorMatrixEffect(Color color);
};

NAMESPACE_DOGEENGINE_END