#pragma once
#include <vector>
#include "Vector2.h"
#include "Structures.h"
#include "Collider.h"

class BoxCollider : public Collider
{
	float width;
	float height;
public:

	BoxCollider();

	void SetSize(float width, float height);
	// SpriteRenderer의 사이즈를 참고합니다.
	void SetSizeAutomatically();

	float GetWidth();
	float GetHeight();
	float GetDiagonalSquare();

	// 네 꼭지점의 좌표를 반환함.
	std::vector<Vector2> GetPoints();

	// OBB인 경우 바운딩 박스의 좌하단 꼭지점 좌표를 반환
	Vector2 GetMinPosition(bool ignoreRotation = false);
	// OBB인 경우 바운딩 박스의 우상단 꼭지점 좌표를 반환
	Vector2 GetMaxPosition(bool ignoreRotation = false);

	RectF GetBoundingBoxRect();

	void DrawBounds(Color color = Color{ 1, 0, 0, 1 }) override;
};