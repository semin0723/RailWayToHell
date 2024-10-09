#pragma once
#include <string>
#include <string_view>

class Object;
class Scene;

class Component
{
	friend class Object;

	Object* ownedObject;
	std::string ComponentType;
	int ComponentID;
public:
	bool enable = true;

	virtual ~Component()
	{
		ownedObject = nullptr;
	}

	void SetComponentEnable(bool enable);

	void SetOwner(Object* _obj);

	Object* GetOwner() const;
	template<class T = Object>
	T* GetOwner() const { return (T*)GetOwner(); }

	// 임시 제거
	/*Scene* GetOwnedScene() const;
	template<class T = Scene>
	T* GetOwnedScene() const { return (T*)GetOwnedScene(); }*/

	void SetID(int id);
	int GetID();
	std::string_view GetComponentType();
};