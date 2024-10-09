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

	// ���İ� ��ȸ�� ���� ������Ʈ �����̳�. std::multiset�� ���� ���ǻ��� �ߺ��� ����ϴ� ����-�� Ʈ�� ������ ���� ���� ���¸� �����Ѵ�.
	// std::set�� �����͸� ������ �⺻������ �ּҸ� �������� ����������, ������ ���� ���� �Լ��� ������ ��� �ش� ������ ������ �� (�ּҰ� �ٸ� ��ü��) ������ ��ü�� �Ǵ��ϰ� ���� �ʴ´ٴ� ���� ������ ��. std::multiset�� �̷� �� ���� �ڷᱸ����.
	std::multiset<Object*, DG::CompareObject> objects;
	// �̸����� �˻��ϱ� ���� ������Ʈ �����̳�
	std::unordered_map<std::string, Object*> objectsByName;

	unsigned int nextObjectID = 0;

	DG::TimerFunctionManager* timerFunctionManager = nullptr;
	DG::EventHandler* eventHandler = nullptr;
public:
	Scene();
	virtual ~Scene();
public:
	Camera* mainCamera = nullptr;
	Object* AddAndSetMainCameraObject(); // �ʹ� ����

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

	// Drawing Priority�� �ٲ� �� �������ϱ� ���� �뵵
	void RearrangeObjectOrder(Object* object, int newPriority);

	// dontDestroyOnSceneChange ������Ʈ���� ���� ��ȯ�� �� �߰��ϴ� �뵵
	Object* AddExistingObject(Object* object);

	void Destroy(Object* object);
	void Destroy(std::string name);

	// ��ȯ ����� bool�� ��ȯ
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

	// �������̵� �ʼ�
	virtual void Initialize() = 0;

	// �������̵� �������� ������ ������ �ϰ� (����Ƽó�� ���� ������Ʈ�� ���� ��� ������ ó���� ���� ����)
	virtual void Update();
	virtual void FixedUpdate();
	virtual void LateUpdate();
	virtual void Draw();

	// �������̵� ����
	virtual void OnLoseFocus() {} // â�� ��Ŀ���� �Ҿ��� �� (WM_KILLFOCUS)
	virtual void OnRecoverFocus() {} // â�� ��Ŀ���� ����� �� (WM_SETFOCUS)
	virtual void OnEnterSizeMove() {} // â�� ����ǥ������ ��� �̵���Ű�ų� �׵θ��� ��� ũ�⸦ �ٲٱ� �������� �� (WM_ENTERSIZEMOVE)
	virtual void OnExitSizeMove() {} // â�� �̵�, ũ�� ���� ������ ������ �� (WM_EXITSIZEMOVE)


	// --------------- Physics? -----------------
	void SimplePhysics(const std::vector<Rigidbody2D*>& bodies, const std::unordered_set<Collider*>& rbColliders, const std::unordered_set<Collider*>& allColliders);
	void PhysicsPhase1_SettleObjectPosition(const std::vector<Rigidbody2D*>& bodies);
	void PhysicsPhase2_CheckCollision(const std::vector<Rigidbody2D*>& bodies, const std::unordered_set<Collider*>& rbColliders, const std::unordered_set<Collider*>& allColliders);
	void Physics_RearrangePosition(Collider* rbCollider, Collider* other, bool rearrange_other_too = false);
};







template <typename T>
T* Scene::_AddObject(std::string name, Vector3 position, ObjectLayer layer)
{
	// nullptr�� ��ȯ�ϴ� �ͺ��� ������ �ܰ迡�� �ƿ� �����ع����� �� ���� �ʳ�?
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