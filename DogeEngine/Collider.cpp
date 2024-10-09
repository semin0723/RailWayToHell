#include "Collider.h"
#include "CollisionCheck.h"
#include "Vector3.h"
#include "Object.h"
#include "Transform.h"
#include "Camera.h"
#include "Input.h"


bool Collider::IsCollideWith(Collider* _collider)
{
	// 첫 번째가 밀어내는 방향의 기준이 되는 rbCollider, 두 번째가 다른 콜라이더
	return DG::CollisionCheck::CheckBetweenColliders(this, _collider);
}
bool Collider::IsCollideWith(Vector2 point)
{
	return DG::CollisionCheck::CheckBetweenColliderAndPoint(this, point);
}
bool Collider::IsCollideWith(Vector2 lineStart, Vector2 lineEnd)
{
	return DG::CollisionCheck::CheckBetweenColliderAndLine(this, lineStart, lineEnd);
}
bool Collider::IsCollideWithCursor()
{
	Vector2 mousePoint = Camera::ScreenToWorldPoint(Input::GetMousePos());
	return DG::CollisionCheck::CheckBetweenColliderAndPoint(this, mousePoint);
}
float Collider::GetOverlapAmountBySATWith(Collider* _collider, Vector2 moveDir)
{
	return DG::CollisionCheck::GetOverlapAmountBySAT(this, _collider, moveDir);
}
void Collider::SetAnchor(float x, float y)

{
	anchor.x = x;
	anchor.y = y;
}
Vector2 Collider::GetAnchor()
{
	Vector3 scale = GetOwner()->transform->GetScale();
	return Vector2{ anchor.x * scale.x, anchor.y * scale.y };
}
Vector2 Collider::GetCenterPosition()
{
	Vector3 pos = GetOwner()->transform->GetPosition();
	Vector3 _anchor = GetAnchor();
	return (pos + _anchor).ToVector2();
}
ColliderType Collider::GetColliderType()
{
	return type;
}