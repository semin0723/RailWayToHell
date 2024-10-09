#pragma once
#include <functional>
#include "Component.h"
#include "Collision.h"

class TimerFunction;

class Script : public Component
{
	friend class Scene;
	friend class Object;

	bool started = false;
	bool initializeOnSceneChange = false;
public:
	// GetOwner()->DoSameThing을 호출하는 방식보다 더 좋은 방식이 있지 않을까?
	void SetObjectEnable(bool enable);
	void SetDontDestroyOnSceneChange(bool dontDestroy, bool initializeOnSceneChange = false);
	void AddTag(const char* _tag);
	void RemoveTag(const char* _tag);
	bool CompareTag(const char* _tag);

	// 헤더 순서 조정 필요함. 일단 보류
	/*template <typename ComponentType>
	ComponentType* AddComponent();
	template <typename ComponentType>
	ComponentType* GetComponent();*/

	TimerFunction* AddTimerFunction(std::function<void(bool)> func, bool duration_mode, float time_para, int count_para);

	void Kill();

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void FixedUpdate() {}
	virtual void LateUpdate() {}

	virtual void OnDestroy() {}

	virtual void OnCollisionEnter(Collision& collision) {}
	virtual void OnCollisionStay(Collision& collision) {}
	virtual void OnCollisionExit(Collision& collision) {}

	virtual void OnTriggerEnter(Collision& collision) {}
	virtual void OnTriggerStay(Collision& collision) {}
	virtual void OnTriggerExit(Collision& collision) {}
};

/*template<typename ComponentType>
inline ComponentType* Script::AddComponent()
{
	return GetOwner()->AddComponent<ComponentType>();
}
template<typename ComponentType>
inline ComponentType* Script::GetComponent()
{
	return GetOwner()->GetComponent<ComponentType>();
}*/

