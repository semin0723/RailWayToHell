#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include "defines.h"
#include "Component.h"
#include "Collision.h"
#include "Vector3.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "Transform.h"
#include "Script.h"


class TimerFunction;
class Physics;
class Transform;

enum class ObjectLayer { Object, UI };

class Object
{
	friend Scene;
	friend Physics;

	std::string name;
	std::unordered_set<std::string> tags;
	Scene* ownedScene;
	int ObjectID;
	ObjectLayer layer;

	//DGCommon::Array<Component*> components;
	std::unordered_map<std::string, std::vector<Component*>> components;
	std::vector<Script*> scriptComponents;
	unsigned int nextComponentID = 0;

	bool enable = true;
	bool started = false; // 루프 중간에 추가된 오브젝트가 초기화(Start)되지 않았는데 다른 함수를 호출하지 않도록 initialized == false인지 체크해줘야 한다.
	bool destroy = false;
	bool dontDestroyOnSceneChange = false;
	bool initializeOnSceneChange = false;
	int drawingPriority = 0;
public:
	Transform* transform;

	void SetEnable(bool enable);
	bool GetEnable();
	bool GetParentEnable();
	void SetStarted(bool initialized);
	bool GetStarted();
	void SetDestroy(bool destroy);
	bool GetDestroy();
	void SetDontDestroyOnSceneChange(bool dontDestroy, bool initializeOnSceneChange = false, bool isRecursivelyToChild = true);
	bool GetDontDestroyOnSceneChange();
	void SetInitializeOnSceneChange(bool initializeOnSceneChange);
	bool GetInitializeOnSceneChange();

	void SetDrawingPriority(int priority);
	// 자식들의 drawing priority를 재귀적으로 설정합니다.
	void SetChildsDrawingPriority(int priority);
	int GetDrawingPriority();

	void InitializeObjectState();

	// 스크립트 컴포넌트 때문에 Start, Update, FixedUpdate, LateUpdate, OnCollision 전부 Object의 멤버함수로 이동해서 동작하는 걸로
	void AwakeObject();
	void StartObject();
	void UpdateObject();
	void FixedUpdateObject();
	void LateUpdateObject();
	void OnDestroyObject();

	void OnCollisionEnterObject(Collision& collision);
	void OnCollisionStayObject(Collision& collision);
	void OnCollisionExitObject(Collision& collision);

	void OnTriggerEnterObject(Collision& collision);
	void OnTriggerStayObject(Collision& collision);
	void OnTriggerExitObject(Collision& collision);



	void SetOwner(Scene* _scene);
	Scene* GetOwner() const;
	template<class T = Scene>
	T* GetOwner() const { return (T*)GetOwner(); }



	template <typename T>
	T* AddObject(std::string name, Vector3 position);
	template <typename T>
	T* AddObject(std::string name);
	template <typename T>
	T* AddObject(Vector3 position);
	template <typename T>
	T* AddObject();
	template <typename T>
	T* AddUIObject(std::string name, Vector3 position);
	template <typename T>
	T* AddUIObject(std::string name);
	template <typename T>
	T* AddUIObject(Vector3 position);
	template <typename T>
	T* AddUIObject();



	bool ChangeName(std::string name);
	std::string GetName();

	void AddTag(std::string tag);
	void RemoveTag(std::string tag);
	bool CompareTag(std::string tag);
	std::unordered_set<std::string>& GetTags();

	void SetID(int id);
	int GetID();
	void SetObjectLayer(ObjectLayer layer);
	ObjectLayer GetObjectLayer();


private:
	template <typename ComponentType>
	ComponentType* _AddComponent();
public:
	template <typename ComponentType>
	ComponentType* AddComponent();
	template <>
	BoxCollider* AddComponent<BoxCollider>();
	template <>
	CircleCollider* AddComponent<CircleCollider>();

	// 컴포넌트 이름으로 찾기 (이름으로 찾지 못 한 경우 자동으로 dynamic casting이 가능한지 재차 체크함)
	template <typename ComponentType>
	ComponentType* GetComponent();
	// dynamic casting으로 찾기
	template <typename ComponentType>
	ComponentType* GetComponentPolymorphic();

	// Collider에 대해 호출하면 모든 종류의 콜라이더를 대상으로 찾음
	template <>
	Collider* GetComponent<Collider>();

	template <typename ComponentType>
	std::vector<ComponentType*> GetComponents();
	// Collider에 대해 호출하면 모든 종류의 콜라이더를 찾아 반환함
	template <>
	std::vector<Collider*> GetComponents<Collider>();
private:
	std::vector<Collider*> GetAllColliders();
	Collider* GetCollider();
public:

	//Component* GetComponentByID(int id);

	TimerFunction* AddTimerFunction(std::function<void(bool)> func, bool duration_mode, float time_para, int count_para);
	TimerFunction* AddRealTimeTimerFunction(std::function<void(bool)> func, bool duration_mode, float time_para, int count_para);


	virtual void Awake() {}
	virtual void Start() {}
	virtual void Draw() {}
	virtual void Update() {}
	virtual void FixedUpdate() {}
	virtual void LateUpdate() {}
	virtual void OnDestroy() {}

	// DontDestroyOnSceneChange 오브젝트에게만 호출됨
	virtual void AfterSceneChange() {}
	virtual void BeforeSceneChange() {}

	virtual void OnCollisionEnter(Collision& collision) {}
	virtual void OnCollisionStay(Collision& collision) {}
	virtual void OnCollisionExit(Collision& collision) {}

	virtual void OnTriggerEnter(Collision& collision) {}
	virtual void OnTriggerStay(Collision& collision) {}
	virtual void OnTriggerExit(Collision& collision) {}


	void Kill();

	Object();
	virtual ~Object();
};







template<typename ComponentType>
inline ComponentType* Object::_AddComponent()
{
	if (!std::is_base_of<Component, ComponentType>::value)
	{
		Assert(false, "unvalid type to convert");
		return nullptr;
	}

	Component* component = new ComponentType;
	component->SetID(nextComponentID++);
	component->SetOwner(this);

	std::string type = typeid(ComponentType).name();
	component->ComponentType = type;
	components[type].push_back(component);

	if (std::is_base_of<Script, ComponentType>::value)
	{
		Script* script = dynamic_cast<Script*>(component);
		scriptComponents.push_back(script);
		script->Awake();
	}

	return static_cast<ComponentType*>(component);
}

template <typename ComponentType>
inline ComponentType* Object::AddComponent()
{
	return _AddComponent<ComponentType>();
}
template <>
inline BoxCollider* Object::AddComponent<BoxCollider>()
{
	BoxCollider* box = _AddComponent<BoxCollider>();
	static_cast<Collider*>(box)->type = ColliderType::Box;

	return box;
}
template <>
inline CircleCollider* Object::AddComponent<CircleCollider>()
{
	CircleCollider* circle = _AddComponent<CircleCollider>();
	static_cast<Collider*>(circle)->type = ColliderType::Circle;

	return circle;
}


// 무효 컴포넌트 거르는 건 GetComponent 류 함수에서 처리해야 함
template <typename ComponentType>
inline ComponentType* Object::GetComponent()
{
	std::string type = typeid(ComponentType).name();
	if (components[type].empty())
	{
		return GetComponentPolymorphic<ComponentType>();
	}

	if (std::is_base_of<Component, ComponentType>::value)
	{
		ComponentType* component = nullptr;
		for (auto comp : components[type])
		{
			//if (comp->enable == true)
			//{
				component = static_cast<ComponentType*>(comp);
				break;
			//}
		}

		return component;
	}
	else return nullptr;

}

template <typename ComponentType>
ComponentType* Object::GetComponentPolymorphic() 
{
	for (auto p : components)
	{
		for (auto comp : p.second)
		{
			//if (comp->enable == false) continue;

			ComponentType* t;
			t = dynamic_cast<ComponentType*>(comp);
			if (t != nullptr) return t;
		}
	}

	return nullptr;
}


template <>
inline Collider* Object::GetComponent<Collider>()
{
	return GetCollider();
}



template <typename ComponentType>
inline std::vector<ComponentType*> Object::GetComponents()
{
	std::vector<ComponentType*> result;

	std::string type = typeid(ComponentType).name();
	if (components[type].empty()) return std::vector<ComponentType*>{};

	if (std::is_base_of<Component, ComponentType>::value)
	{
		for (Component* c : components[type])
		{
			if (c->enable == true)
			{
				ComponentType* component = static_cast<ComponentType*>(c);
				result.push_back(component);
			}
		}
		return result;
	}
	else return std::vector<ComponentType*>{};
}
template <>
inline std::vector<Collider*> Object::GetComponents<Collider>()
{
	return GetAllColliders();
}



// 외부에서 템플릿 메서드를 호출하는 문법이 상당히 희안함
template<typename T>
inline T* Object::AddObject(std::string name, Vector3 position)
{
	return GetOwner()->template AddObject<T>(name, position);
}
template<typename T>
inline T* Object::AddObject(std::string name)
{
	return GetOwner()->template AddObject<T>(name);
}
template<typename T>
inline T* Object::AddObject(Vector3 position)
{
	return GetOwner()->template AddObject<T>(position);
}
template<typename T>
inline T* Object::AddObject()
{
	return GetOwner()->template AddObject<T>();
}
template<typename T>
inline T* Object::AddUIObject(std::string name, Vector3 position)
{
	return GetOwner()->template AddUIObject<T>(name, position);
}
template<typename T>
inline T* Object::AddUIObject(std::string name)
{
	return GetOwner()->template AddUIObject<T>(name);
}
template<typename T>
inline T* Object::AddUIObject(Vector3 position)
{
	return GetOwner()->template AddUIObject<T>(position);
}
template<typename T>
inline T* Object::AddUIObject()
{
	return GetOwner()->template AddUIObject<T>();
}
