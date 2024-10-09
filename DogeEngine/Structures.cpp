#include "Structures.h"

D2D1_SIZE_F SizeF::ToD2D1Size()
{
	return D2D1_SIZE_F{ width, height };
}

D2D1_RECT_F RectF::ToD2D1Rect()
{
	return D2D1_RECT_F{ left, top, right, bottom };
}

D2D1::ColorF ColorByte::ConvertToD2DColorF()
{
	return D2D1::ColorF{ (float)R / 255, (float)G / 255, (float)B / 255, (float)A / 255 };
}

D2D1::ColorF Color::ConvertToD2DColorF()
{
	return D2D1::ColorF{ R, G, B, A };
}