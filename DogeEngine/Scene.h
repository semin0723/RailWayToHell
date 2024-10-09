#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <memory>
#include "defines.h"
#include "Vector3.h"
#include "Object.h"

class Collider;
class SpriteRenderer;
class Camera;
class Transform;
class AudioManager;
class Rigidbody2D;

class TimerFunction;
NAMESPACE_DOGEENGINE_BEGIN
class TimerFunctionManager;
class EventHandler;
class D2DRenderer;

struct CompareObject
{
	bool operator()(Object* a, Object* b) const;
};
NAMESPACE_DOGEENGINE_END



class Scene
{
protected:
	bool initialized = false;

	// 정렬과 순회를 위한 오브젝트 컨테이너. std::multiset은 정렬 조건상의 중복을 허용하는 레드-블랙 트리 구조로 완전 정렬 상태를 유지한다.
	// std::set에 포인터를 담으면 기본적으로 주소를 기준으로 정렬하지만, 별도의 정렬 조건 함수를 지정할 경우 해당 조건이 동일할 때 (주소가 다른 객체라도) 동일한 객체로 판단하고 담지 않는다는 점을 주의할 것. std::multiset은 이럴 때 쓰는 자료구조다.
	std::multiset<Object*, DG::CompareObject> objects;
	// 이름으로 검색하기 위한 오브젝트 컨테이너
	std::unordered_map<std::string, Object*> objectsByName;

	unsigned int nextObjectID = 0;

	DG::TimerFunctionManager* timerFunctionManager = nullptr;
	DG::EventHandler* eventHandler = nullptr;
public:
	Scene();
	virtual ~Scene();
public:
	Camera* mainCamera = nullptr;
	Object* AddAndSetMainCameraObject(); // 너무 구림

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>> additionalRenderer;
	void AddAdditionalRenderer(std::string name, int width, int height, Camera* camera);
	void AddAdditionalRenderer(std::string name, int width, int height, Object* cameraObject);
	void DeleteAdditionalRenderer(std::string name);
protected:
	template <typename T>
	T* _AddObject(std::string name, Vector3 position, ObjectLayer layer);
	void DestroyObject(Object* object);
public:
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

	// Drawing Priority를 바꾼 뒤 재정렬하기 위한 용도
	void RearrangeObjectOrder(Object* object, int newPriority);

	// dontDestroyOnSceneChange 오브젝트들을 씬이 전환될 때 추가하는 용도
	Object* AddExistingObject(Object* object);

	void Destroy(Object* object);
	void Destroy(std::string name);

	// 변환 결과를 bool로 반환
	bool ChangeObjectName(Object* obj, std::string name);
	Object* GetObjectByName(std::string name);
	template <typename T>
	T* GetObjectByName(std::string name)
	{
		return static_cast<T*>(GetObjectByName(name));
	}
	Object* GetObjectByTag(const char* tag);
	std::vector<Object*> GetObjectsByTag(const char* tag);
	std::multiset<Object*, DG::CompareObject>& GetObjectList();

	bool GetInitState();
	void InitializeScene();
	DG::EventHandler* GetEventHandler();
	DG::TimerFunctionManager* GetTimerFunctionManager();

	// 오버라이드 필수
	virtual void Initialize() = 0;

	// 오버라이드 권장하지 않으나 가능은 하게 (유니티처럼 씬의 오브젝트를 통해 모든 동작을 처리할 것을 권장)
	virtual void Update();
	virtual void FixedUpdate();
	virtual void LateUpdate();
	virtual void Draw();

	// 오버라이드 가능
	virtual void OnLoseFocus() {} // 창의 포커스를 잃었을 때 (WM_KILLFOCUS)
	virtual void OnRecoverFocus() {} // 창의 포커스를 얻었을 때 (WM_SETFOCUS)
	virtual void OnEnterSizeMove() {} // 창의 제목표시줄을 잡고 이동시키거나 테두리를 잡고 크기를 바꾸기 시작했을 때 (WM_ENTERSIZEMOVE)
	virtual void OnExitSizeMove() {} // 창의 이동, 크기 변경 동작을 멈췄을 때 (WM_EXITSIZEMOVE)


	// --------------- Physics? -----------------
	void SimplePhysics(const std::vector<Rigidbody2D*>& bodies, const std::unordered_set<Collider*>& rbColliders, const std::unordered_set<Collider*>& allColliders);
	void PhysicsPhase1_SettleObjectPosition(const std::vector<Rigidbody2D*>& bodies);
	void PhysicsPhase2_CheckCollision(const std::vector<Rigidbody2D*>& bodies, const std::unordered_set<Collider*>& rbColliders, const std::unordered_set<Collider*>& allColliders);
	void Physics_RearrangePosition(Collider* rbCollider, Collider* other, bool rearrange_other_too = false);
};







template <typename T>
T* Scene::_AddObject(std::string name, Vector3 position, ObjectLayer layer)
{
	// nullptr를 반환하는 것보다 컴파일 단계에서 아예 차단해버리는 게 낫지 않나?
	if (!std::is_base_of<Object, T>::value) return nullptr;

	T* obj = new T;

	// objects
	obj->SetID(nextObjectID);
	obj->SetOwner(this);
	obj->SetObjectLayer(layer);
	nextObjectID++;
	objects.insert(obj);

	// objectsByName
	int tmpNum = 1;
	std::string tempName = name;
	while (true)
	{
		if (objectsByName.find(tempName) == objectsByName.end()) break;
		tempName = std::string(name) + " (" + std::to_string(tmpNum) + ")";
		tmpNum++;
	}
	obj->name = tempName;
	objectsByName.insert(std::make_pair(tempName, obj));

	obj->transform->SetPosition(position);

	obj->AwakeObject();

	return obj;
}

template<typename T>
T* Scene::AddObject(std::string name, Vector3 position)
{
	T* obj = _AddObject<T>(name, position, ObjectLayer::Object);

	return obj;
}
template <typename T>
T* Scene::AddObject(std::string name)
{
	T* obj = _AddObject<T>(name, Vector3(0, 0, 0), ObjectLayer::Object);

	return obj;
}
template<typename T>
T* Scene::AddObject(Vector3 position)
{
	std::string name;
	name = "obj" + std::to_string(nextObjectID);
	T* obj = _AddObject<T>(name, position, ObjectLayer::Object);

	return obj;
}
template <typename T>
T* Scene::AddObject()
{
	std::string name;
	name = "obj" + std::to_string(nextObjectID);
	T* obj = _AddObject<T>(name, Vector3(0, 0, 0), ObjectLayer::Object);

	return obj;
}


template<typename T>
inline T* Scene::AddUIObject(std::string name, Vector3 position)
{
	T* obj = _AddObject<T>(name, position, ObjectLayer::UI);

	return obj;
}
template<typename T>
inline T* Scene::AddUIObject(std::string name)
{
	T* obj = _AddObject<T>(name, Vector3(0, 0, 0), ObjectLayer::UI);

	return obj;
}
template<typename T>
inline T* Scene::AddUIObject(Vector3 position)
{
	std::string name;
	name = "uielement" + std::to_string(nextObjectID);
	T* obj = _AddObject<T>(name, position, ObjectLayer::UI);

	return obj;
}
template<typename T>
inline T* Scene::AddUIObject()
{
	std::string name;
	name = "uielement" + std::to_string(nextObjectID);
	T* obj = _AddObject<T>(name, Vector3(0, 0, 0), ObjectLayer::UI);

	return obj;
}