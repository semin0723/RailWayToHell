#pragma once
#include "DirectXFramework.h"

struct SizeF
{
	float width, height;

	SizeF() : width(0), height(0) {}
	SizeF(float width, float height) : width(width), height(height) {}
	SizeF(D2D1_SIZE_F size) : width(size.width), height(size.height) {}
	D2D1_SIZE_F ToD2D1Size();
};
struct RectF
{
	float left, top, right, bottom;

	RectF() : left(0), top(0), right(0), bottom(0) {}
	RectF(float left, float top, float right, float bottom) : left(left), top(top), right(right), bottom(bottom) {}
	RectF(D2D1_RECT_F rect) : left(rect.left), top(rect.top), right(rect.right), bottom(rect.bottom) {}
	D2D1_RECT_F ToD2D1Rect();
};
struct ColorByte
{
	int R, G, B, A;
	ColorByte(int r, int g, int b, int a) : R(r), G(g), B(b), A(a) {}

	D2D1::ColorF ConvertToD2DColorF();
};
struct Color
{
	float R, G, B, A;

	Color(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) {}

	D2D1::ColorF ConvertToD2DColorF();
};