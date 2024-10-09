#include <string>
#include <iterator>
#include "Object.h"
#include "Scene.h"
#include "Script.h"
#include "Animator.h"
#include "TimerFunctionManager.h"
#include "Transform.h"

void Object::SetEnable(bool enable)
{
	this->enable = enable;
}
bool Object::GetEnable()
{
	//return enable;
	return GetParentEnable();
}
bool Object::GetParentEnable()
{
	if (transform->GetParent() == nullptr) return enable;
	else if (enable == false) return false;
	else
	{
		if (transform->GetParent()->GetOwner()->enable == false) return false;
		return transform->GetParent()->GetOwner()->GetParentEnable();
	}
}
void Object::SetStarted(bool started)
{
	this->started = started;
}
bool Object::GetStarted()
{
	return started;
}
void Object::SetDestroy(bool destroy)
{
	this->destroy = destroy;
}
bool Object::GetDestroy()
{
	return destroy;
}
void Object::SetDontDestroyOnSceneChange(bool dontDestroy, bool initializeOnSceneChange, bool isRecursivelyToChild)
{
	this->dontDestroyOnSceneChange = dontDestroy;
	this->initializeOnSceneChange = initializeOnSceneChange;

	if (isRecursivelyToChild)
	{
		for (auto& item : *transform->GetChild())
		{
			item->GetOwner()->SetDontDestroyOnSceneChange(dontDestroy, initializeOnSceneChange, isRecursivelyToChild);
		}
	}
}
bool Object::GetDontDestroyOnSceneChange()
{
	return dontDestroyOnSceneChange;
}
void Object::SetInitializeOnSceneChange(bool initializeOnSceneChange)
{
	this->initializeOnSceneChange = initializeOnSceneChange;
}
bool Object::GetInitializeOnSceneChange()
{
	return initializeOnSceneChange;
}
void Object::SetDrawingPriority(int priority)
{
	GetOwner()->RearrangeObjectOrder(this, priority);
}
void Object::SetChildsDrawingPriority(int priority)
{
	for (auto child : *transform->GetChild())
	{
		child->GetOwner()->SetDrawingPriority(priority);
		child->GetOwner()->SetChildsDrawingPriority(priority);
	}
}
int Object::GetDrawingPriority()
{
	return drawingPriority;
}
void Object::InitializeObjectState()
{
	enable = true;
	started = false;
	destroy = false;
}



void Object::AwakeObject()
{
	Awake();
	// Script의 Awake는 Script 컴포넌트를 부착하는 순간 실행
}

void Object::StartObject()
{
	Start();
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == true) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->Start();
		scriptComponents[i]->started = true;
	}

	started = true;
}
void Object::UpdateObject()
{
	Update();
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->enable == false) continue;
		if (scriptComponents[i]->started == false)
		{
			scriptComponents[i]->Start();
			scriptComponents[i]->started = true;
		}
		scriptComponents[i]->Update();
	}

	// Animation ~ animator 내부에서 deltaTime 누적하므로 Update 안에서 해야 됨
	Animator* animator = GetComponent<Animator>();
	if (animator != nullptr && animator->isRunOnTime == true && animator->enable == true)
	{
		animator->UpdateFrameByTime();
	}
}
void Object::FixedUpdateObject()
{
	FixedUpdate();
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == false) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->FixedUpdate();
	}
}
void Object::LateUpdateObject()
{
	LateUpdate();
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == false) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->LateUpdate();
	}
}
void Object::OnDestroyObject()
{
	OnDestroy();
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == false) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->OnDestroy();
	}
}
void Object::OnCollisionEnterObject(Collision& collision)
{
	OnCollisionEnter(collision);
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == false) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->OnCollisionEnter(collision);
	}
}
void Object::OnCollisionStayObject(Collision& collision)
{
	OnCollisionStay(collision);
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == false) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->OnCollisionStay(collision);
	}
}
void Object::OnCollisionExitObject(Collision& collision)
{
	OnCollisionExit(collision);
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == false) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->OnCollisionExit(collision);
	}
}

void Object::OnTriggerEnterObject(Collision& collision)
{
	OnTriggerEnter(collision);
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == false) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->OnTriggerEnter(collision);
	}
}

void Object::OnTriggerStayObject(Collision& collision)
{
	OnTriggerStay(collision);
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == false) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->OnTriggerStay(collision);
	}
}

void Object::OnTriggerExitObject(Collision& collision)
{
	OnTriggerExit(collision);
	for (int i = 0; i < scriptComponents.size(); i++)
	{
		if (scriptComponents[i]->started == false) continue;
		if (scriptComponents[i]->enable == false) continue;
		scriptComponents[i]->OnTriggerExit(collision);
	}
}





void Object::SetOwner(Scene* _scene)
{
	ownedScene = _scene;
}
Scene* Object::GetOwner() const
{
	return ownedScene;
}
bool Object::ChangeName(std::string name)
{
	return GetOwner()->ChangeObjectName(this, name);
}
std::string Object::GetName()
{
	return name;
}

void Object::AddTag(std::string _tag)
{
	tags.insert(_tag);
}
void Object::RemoveTag(std::string _tag)
{
	tags.erase(_tag);
}
bool Object::CompareTag(std::string _tag)
{
	return tags.find(_tag) != tags.end();
}

std::unordered_set<std::string>& Object::GetTags()
{
	return tags;
}


void Object::SetID(int id)
{
	ObjectID = id;
}

int Object::GetID()
{
	return ObjectID;
}

void Object::SetObjectLayer(ObjectLayer layer)
{
	this->layer = layer;
}

ObjectLayer Object::GetObjectLayer()
{
	return layer;
}






/*Component* Object::GetComponentByID(int id)
{
	for (int i = 0; i < components.Size(); i++)
	{
		if (id == components[i]->GetID())
		{
			return components[i];
		}
	}
	return nullptr;
}*/

std::vector<Collider*> Object::GetAllColliders()
{
	std::vector<Collider*> result;

	auto boxes = GetComponents<BoxCollider>();
	auto circles = GetComponents<CircleCollider>();

	std::copy(boxes.begin(), boxes.end(), std::back_inserter(result));
	std::copy(circles.begin(), circles.end(), std::back_inserter(result));

	return result;
}

Collider* Object::GetCollider()
{
	BoxCollider* boxCollider = GetComponent<BoxCollider>();
	if (boxCollider != nullptr) return boxCollider;
	CircleCollider* circleCollider = GetComponent<CircleCollider>();
	if (circleCollider != nullptr) return circleCollider;

	return nullptr;
}

TimerFunction* Object::AddTimerFunction(std::function<void(bool)> func, bool duration_mode, float time_para, int count_para)
{
	DG::TimerFunctionManager* tfm = ownedScene->GetTimerFunctionManager();
	return tfm->RegisterTimerFunction(this, func, duration_mode, time_para, count_para);
}

TimerFunction* Object::AddRealTimeTimerFunction(std::function<void(bool)> func, bool duration_mode, float time_para, int count_para)
{
	DG::TimerFunctionManager* tfm = ownedScene->GetTimerFunctionManager();
	return tfm->RegisterRealTimeTimerFunction(this, func, duration_mode, time_para, count_para);
}

void Object::Kill()
{
	destroy = true;
	enable = false;

	for (auto c : *transform->GetChild())
	{
		c->GetOwner()->Kill();
	}
}

Object::Object()
{
	transform = AddComponent<Transform>();
}

Object::~Object()
{
	ownedScene = nullptr;
	for (auto c : components)
	{
		std::vector<Component*>& comps = c.second;
		for (int i = 0; i < comps.size(); ++i)
		{
			delete comps[i];
		}
	}
	//for (int i = 0; i < components.Size(); i++)
	//{
	//	// scriptComponents는 components에도 포함됨
	//	delete components[i];
	//}
}