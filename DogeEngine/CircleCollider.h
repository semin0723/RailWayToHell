#pragma once
#include "Collider.h"
#include "Structures.h"

class CircleCollider : public Collider
{
	float radius;
public:

	CircleCollider();

	void SetSize(float radius);
	// SpriteRenderer의 사이즈를 참고합니다.
	void SetSizeAutomatically();

	float GetRadius();

	virtual void DrawBounds(Color color = Color{ 1, 0, 0, 1 }) override;
};