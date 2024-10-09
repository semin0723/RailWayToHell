#include "CircleCollider.h"
#include "Object.h"
#include "SpriteRenderer.h"
#include "Vector3.h"
#include "DrawingManager.h"

CircleCollider::CircleCollider() : radius(50)
{
}
void CircleCollider::SetSize(float _radius)
{
	radius = _radius;
}
void CircleCollider::SetSizeAutomatically()
{
	SpriteRenderer* renderer = GetOwner()->GetComponent<SpriteRenderer>();
	if (renderer != nullptr)
	{
		radius = renderer->GetWidth() >= renderer->GetHeight() ? renderer->GetWidth() / 2 : renderer->GetHeight() / 2;
	}
}
float CircleCollider::GetRadius()
{
	Vector3 scale = GetOwner()->transform->GetScale();
	return radius * scale.x;
}
void CircleCollider::DrawBounds(Color color)
{
	DrawingManager::DrawCircle(GetCenterPosition(), GetRadius(), Color{ 1,0,0,1 }, 1.0f);
}