#include "Component.h"
#include "Object.h"

void Component::SetComponentEnable(bool enable)
{
	this->enable = enable;
}

void Component::SetOwner(Object* _obj)
{
	ownedObject = _obj;
}
Object* Component::GetOwner() const
{
	return ownedObject;
}
//Scene* Component::GetOwnedScene() const
//{
//	return ownedObject->GetOwner();
//}

void Component::SetID(int id)
{
	ComponentID = id;
}

int Component::GetID()
{
	return ComponentID;
}

std::string_view Component::GetComponentType()
{
	return ComponentType;
}