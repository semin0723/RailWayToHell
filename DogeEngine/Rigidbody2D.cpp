#include "Rigidbody2D.h"
#include "Object.h"
#include "Time.h"

void Rigidbody2D::MoveObject()
{
	GetOwner()->transform->Translate(velocity * Time::GetFixedDeltaTime());
}
void Rigidbody2D::MoveBackObject()
{
	GetOwner()->transform->Translate(-velocity * Time::GetFixedDeltaTime());
}
void Rigidbody2D::AddForce(Vector3 force, ForceMode mode)
{
	if (mode == ForceMode::Force)
	{
		velocity = velocity + force / mass * Time::GetFixedDeltaTime();
	}
	else if (mode == ForceMode::Impulse)
	{
		velocity = velocity + force / mass;
	}
}