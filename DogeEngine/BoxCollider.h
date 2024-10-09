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
	// SpriteRenderer�� ����� �����մϴ�.
	void SetSizeAutomatically();

	float GetWidth();
	float GetHeight();
	float GetDiagonalSquare();

	// �� �������� ��ǥ�� ��ȯ��.
	std::vector<Vector2> GetPoints();

	// OBB�� ��� �ٿ�� �ڽ��� ���ϴ� ������ ��ǥ�� ��ȯ
	Vector2 GetMinPosition(bool ignoreRotation = false);
	// OBB�� ��� �ٿ�� �ڽ��� ���� ������ ��ǥ�� ��ȯ
	Vector2 GetMaxPosition(bool ignoreRotation = false);

	RectF GetBoundingBoxRect();

	void DrawBounds(Color color = Color{ 1, 0, 0, 1 }) override;
};