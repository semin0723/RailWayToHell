#include "BoxCollider.h"
#include "SpriteRenderer.h"
#include "Object.h"
#include "Math.h"
#include "DrawingManager.h"

BoxCollider::BoxCollider() : width(100), height(100)
{

}
void BoxCollider::SetSize(float _width, float _height)
{
	width = _width;
	height = _height;
}

void BoxCollider::SetSizeAutomatically()
{
	SpriteRenderer* renderer = GetOwner()->GetComponent<SpriteRenderer>();
	if (renderer != nullptr)
	{
		width = renderer->GetWidth();
		height = renderer->GetHeight();
	}
}
float BoxCollider::GetWidth()
{
	Vector3 scale = GetOwner()->transform->GetScale();
	return width * scale.x;
}
float BoxCollider::GetHeight()
{
	Vector3 scale = GetOwner()->transform->GetScale();
	return height * scale.y;
}
float BoxCollider::GetDiagonalSquare()
{
	return pow(GetWidth(), 2) + pow(GetHeight(), 2);
}
std::vector<Vector2> BoxCollider::GetPoints()
{
	Vector3 pos = GetCenterPosition();
	Vector3 scale = GetOwner()->transform->GetScale();
	float radian = Math::Deg2Rad(GetOwner()->transform->GetRotation().z);

	float halfWidth = width * scale.x / 2.f;
	float halfHeight = height * scale.y / 2.f;

	Vector3 lb{ -halfWidth, -halfHeight, 0 };
	Vector3 lt{ -halfWidth, halfHeight, 0 };
	Vector3 rt{ halfWidth, halfHeight, 0 };
	Vector3 rb{ halfWidth, -halfHeight, 0 };

	if (!Math::Approximate(radian, 0.0f))
	{
		lb = lb.Rotate(radian);
		lt = lt.Rotate(radian);
		rt = rt.Rotate(radian);
		rb = rb.Rotate(radian);
	}

	std::vector<Vector2> points;
	points.push_back(lb + pos);
	points.push_back(lt + pos);
	points.push_back(rt + pos);
	points.push_back(rb + pos);

	return points;
}

Vector2 BoxCollider::GetMinPosition(bool ignoreRotation)
{
	if (ignoreRotation)
	{
		Vector3 pos = GetOwner()->transform->GetPosition();
		Vector3 scale = GetOwner()->transform->GetScale();
		Vector3 _anchor = GetAnchor();

		return Vector2{ pos.x + _anchor.x - width * scale.x / 2, pos.y + _anchor.y - height * scale.y / 2 };
	}
	else
	{
		float radian = Math::Deg2Rad(GetOwner()->transform->GetRotation().z);

		if (Math::Approximate(radian, 0.0f))
		{
			Vector3 pos = GetOwner()->transform->GetPosition();
			Vector3 scale = GetOwner()->transform->GetScale();
			Vector3 _anchor = GetAnchor();

			return Vector2{ pos.x + _anchor.x - width * scale.x / 2, pos.y + _anchor.y - height * scale.y / 2 };
		}
		else
		{
			RectF bounds = GetBoundingBoxRect();
			return Vector2{ bounds.left, bounds.bottom };
		}
	}
}
Vector2 BoxCollider::GetMaxPosition(bool ignoreRotation)
{
	if (ignoreRotation)
	{
		Vector3 pos = GetOwner()->transform->GetPosition();
		Vector3 scale = GetOwner()->transform->GetScale();
		Vector3 _anchor = GetAnchor();

		return Vector2{ pos.x + _anchor.x + width * scale.x / 2, pos.y + _anchor.y + height * scale.y / 2 };
	}
	else
	{
		float rotation = GetOwner()->transform->GetRotation().z;

		if (Math::Approximate(rotation, 0.0f))
		{
			Vector3 pos = GetOwner()->transform->GetPosition();
			Vector3 scale = GetOwner()->transform->GetScale();
			Vector3 _anchor = GetAnchor();

			return Vector2{ pos.x + _anchor.x + width * scale.x / 2, pos.y + _anchor.y + height * scale.y / 2 };
		}
		else
		{
			RectF bounds = GetBoundingBoxRect();
			return Vector2{ bounds.right, bounds.top };
		}
	}
}

RectF BoxCollider::GetBoundingBoxRect()
{
	float rotation = GetOwner()->transform->GetRotation().z;
	if (Math::Approximate(rotation, 0.0f))
	{
		Vector2 min = GetMinPosition();
		Vector2 max = GetMaxPosition();
		return RectF{ min.x, max.y, max.x, min.y };
	}
	else
	{
		std::vector<Vector2> points = GetPoints();
		float left = points[0].x;
		float right = points[0].x;
		float top = points[0].y;
		float bottom = points[0].y;
		for (Vector2 point : points)
		{
			left = left > point.x ? point.x : left;
			right = right < point.x ? point.x : right;
			top = top < point.y ? point.y : top;
			bottom = bottom > point.y ? point.y : bottom;
		}

		return RectF{ left, top, right, bottom };
	}
}

void BoxCollider::DrawBounds(Color color)
{
	DrawingManager::DrawSquare(GetPoints(), Color{ 1, 0, 0, 1 });

	/*Vector2 min = GetMinPosition();
	Vector2 max = GetMaxPosition();

	DrawingManager::DrawOrientedSqaure(min, max, GetOwner()->transform->GetRotation().z, Color{ 1, 0, 0, 1 }, 1.0f);*/
}