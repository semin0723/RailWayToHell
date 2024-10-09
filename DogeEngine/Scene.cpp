#include "Scene.h"
#include "TimerFunctionManager.h"
#include "EventHandler.h"
#include "Debug.h"
#include "Camera.h"
#include "Core.h"
#include "Rigidbody2D.h"
#include "PhysicsConstant.h"
#include "CollisionCheck.h"
#include "Physics.h"
#include "Math.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"


NAMESPACE_DOGEENGINE_BEGIN
bool CompareObject::operator()(Object* a, Object* b) const
{
	if (a->GetDrawingPriority() == b->GetDrawingPriority())
	{
		return (a->GetID() < b->GetID());
	}
	else
	{
		return a->GetDrawingPriority() < b->GetDrawingPriority();
	}
}
NAMESPACE_DOGEENGINE_END


Scene::Scene()
{
	timerFunctionManager = new DG::TimerFunctionManager();
	eventHandler = new DG::EventHandler();
}
Scene::~Scene()
{
	for (auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		if ((*iter)->GetDontDestroyOnSceneChange() == true) continue;
		(*iter)->OnDestroy();
		delete (*iter);
	}

	if (timerFunctionManager != nullptr)
	{
		delete timerFunctionManager;
	}
}

void Scene::DestroyObject(Object* obj)
{
	obj->OnDestroyObject();

	objects.erase(obj);
	objectsByName.erase(obj->GetName());
	timerFunctionManager->DeleteTimerFunction(obj);
	eventHandler->DeleteAllEventListener(obj);

	delete obj;
}
void Scene::RearrangeObjectOrder(Object* object, int newPriority)
{
	auto iter = objects.find(object);
	
	// set�� find�� �� ���� ���� ���� �Լ��� ����ϱ� ������ ���� priority�� �ٲ� ���� ã���� �� ã�´�
	if (objects.find(object) == objects.end())
	{
		Assert(false, "faild to find object");
		return;
	}

	objects.erase(iter);
	object->drawingPriority = newPriority;
	objects.insert(object);
}
Object* Scene::AddExistingObject(Object* obj)
{
	obj->SetID(nextObjectID);
	obj->SetOwner(this);
	nextObjectID++;
	objects.insert(obj);

	int tmpNum = 1;
	std::string tempName = obj->GetName();
	while (true)
	{
		if (objectsByName.find(tempName.c_str()) == objectsByName.end()) break;
		tempName = obj->GetName() + " (" + std::to_string(tmpNum) + ")";
		tmpNum++;
	}
	obj->ChangeName(tempName);
	objectsByName.insert(std::make_pair(tempName, obj));

	return obj;
}
void Scene::Destroy(Object* object)
{
	object->Kill();
}
void Scene::Destroy(std::string name)
{
	auto it = objectsByName.find(name.c_str());
	if (it != objectsByName.end())
	{
		Object* obj = it->second;
		obj->Kill();
	}
}

bool Scene::ChangeObjectName(Object* obj, std::string name)
{
	auto iter = objectsByName.find(obj->GetName().c_str());
	if (iter == objectsByName.end()) return false; // ������Ʈ�� ã�� �� ��

	// std::string�� find�� �״�� �־ ã�� �� �Ұ���? find(name)���� �ϸ� ������ �� �Ѵ�.
	auto iter2 = objectsByName.find(name.c_str());
	if (iter == objectsByName.end()) return false; // �ߺ��� �̸����� ���� �õ�

	objectsByName.erase(obj->GetName());
	objectsByName[name] = obj;

	obj->name = name;
	return true;
}

Object* Scene::GetObjectByName(std::string name)
{
	auto it = objectsByName.find(name.c_str());
	if (it != objectsByName.end())
	{
		return it->second;
	}
	return nullptr;
}
Object* Scene::GetObjectByTag(const char* tag)
{
	for (auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		if ((*iter)->CompareTag(tag)) return (*iter);
	}
	return nullptr;
}
std::vector<Object*> Scene::GetObjectsByTag(const char* tag)
{
	std::vector<Object*> result;
	for (auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		if ((*iter)->CompareTag(tag))
		{
			result.push_back((*iter));
		}
	}
	return result;
}

std::multiset<Object*, DG::CompareObject>& Scene::GetObjectList()
{
	return objects;
}

bool Scene::GetInitState()
{
	return initialized;
}

void Scene::InitializeScene()
{
	if (initialized == false)
	{
		Initialize();

		// Initialize ���� ����ī�޶� �������� ���� ���, �켱 ������Ʈ �߿� ī�޶� ������Ʈ�� �ִ��� ã��, �׷��� ���ٸ� �߰��Ѵ�.
		if (mainCamera == nullptr)
		{
			Debug::Log("ī�޶� ã�� �� ����.");
			for (auto obj : objects)
			{
				Camera* cam = obj->GetComponent<Camera>();
				if (cam != nullptr)
				{
					mainCamera = cam;
					DG::Core::GetInstance().mainRenderer.get()->ChangeCamera(cam);
					break;
				}
			}

			if (mainCamera == nullptr)
			{
				AddAndSetMainCameraObject();
			}
		}

		initialized = true;


		// ���⼭ ������ ������Ʈ�� Start�� ȣ���� �� �Ǹ� Draw() -> Update() ���̶� �� ���� �����
		auto tObjects = objects;
		for (auto iter = tObjects.cbegin(); iter != tObjects.cend(); ++iter)
		{
			Object* obj = (*iter);
			if (obj->GetStarted() == false) obj->StartObject();
		}
	}
}

DG::EventHandler* Scene::GetEventHandler()
{
	return eventHandler;
}

DG::TimerFunctionManager* Scene::GetTimerFunctionManager()
{
	return timerFunctionManager;
}

Object* Scene::AddAndSetMainCameraObject()
{
	Object* obj = AddObject<Object>("MainCamera", Vector3{ 0, 0, 0 });
	mainCamera = obj->AddComponent<Camera>();
	DG::Core::GetInstance().mainRenderer.get()->ChangeCamera(mainCamera);

	return obj;
}
void Scene::AddAdditionalRenderer(std::string name, int width, int height, Camera* camera)
{
	if (additionalRenderer.find(name) != additionalRenderer.end())
	{
		Assert(false, "renderer name already exist");
		return;
	}

	additionalRenderer[name] = std::shared_ptr<DG::D2DRenderer>(new DG::D2DRenderer(width, height, camera));
}
void Scene::AddAdditionalRenderer(std::string name, int width, int height, Object* cameraObject)
{
	Camera* camera = cameraObject->GetComponent<Camera>();

	if (camera == nullptr)
	{
		Assert(false, "failed to find camera component");
		return;
	}
	AddAdditionalRenderer(name, width, height, camera);
}
void Scene::DeleteAdditionalRenderer(std::string name)
{
	auto iter = additionalRenderer.find(name);
	if (iter != additionalRenderer.end())
	{
		additionalRenderer.erase(iter);
	}
}
void Scene::Update()
{
	// Update �������� SetDrawingPriority()�� ���� set �����̳� �� �������� �Ͼ ��� set/map iterator incompatible ���� �߻� / ���纻���� ������.
	auto objectsCopy = objects;
	for (auto iter = objectsCopy.cbegin(); iter != objectsCopy.cend();)
	{
		Object* curObj = (*iter);
		++iter; // ������Ʈ �������� ������ ���� ��츦 �����Ͽ� �ݺ��ڸ� �̸� ����

		if (curObj->GetDestroy() == true)
		{
			DestroyObject(curObj);
			continue;
		}
		if (curObj->GetParentEnable() == false) continue;
		if (curObj->GetStarted() == false) curObj->StartObject();
		curObj->UpdateObject();
	}

	timerFunctionManager->Update();
	eventHandler->Update();

	// �浹üũ�� FixedUpdate���� ����
}



// �浹 üũ�� ���߿� physics thread�� physics loop�� �����Ͽ� �� ������ �� ��
void Scene::FixedUpdate()
{
	std::vector<Rigidbody2D*> bodies;
	std::unordered_set<Collider*> rbColliders; // Rigidbody�� ������ ������Ʈ�� �ݶ��̴�
	std::unordered_set<Collider*> allColliders; // ��� �ݶ��̴�

	// Update �������� SetDrawingPriority()�� ���� set �����̳� �� �������� �Ͼ ��� set/map iterator incompatible ���� �߻� / ���纻���� ������.
	auto objectsCopy = objects;
	for (auto iter = objectsCopy.cbegin(); iter != objectsCopy.cend();)
	{
		Object* curObj = (*iter);
		++iter; // ������Ʈ �������� ������ ���� ��츦 �����Ͽ� �ݺ��ڸ� �̸� ����

		if (curObj->GetParentEnable() == false || curObj->started == false) continue;
		curObj->FixedUpdateObject(); // �ʱ�ȭ�� Update������

		// ObjectLayer::Object ������Ʈ���� ������ ���� �ùķ��̼� ���� ���
		if (curObj->GetObjectLayer() == ObjectLayer::Object)
		{
			// ��ȿ �ݶ��̴��� �˾Ƽ� �ɷ����� �´�.
			std::vector<Collider*> _colliders = curObj->GetComponents<Collider>();
			if (!_colliders.empty())
			{
				// std::inserter�� �ڷᱸ���� insert() �Լ��� ȣ���Ѵ�.
				std::copy(_colliders.begin(), _colliders.end(), std::inserter(allColliders, allColliders.end()));
			}

			Rigidbody2D* rigidbody = curObj->GetComponent<Rigidbody2D>();
			if (rigidbody != nullptr)
			{
				bodies.push_back(rigidbody);
				if (!_colliders.empty())
				{
					std::copy(_colliders.begin(), _colliders.end(), std::inserter(rbColliders, rbColliders.end()));
				}
			}
		}
	}

	SimplePhysics(bodies, rbColliders, allColliders);
}

void Scene::SimplePhysics(const std::vector<Rigidbody2D*>& bodies, const std::unordered_set<Collider*>& rbColliders, const std::unordered_set<Collider*>& allColliders)
{
	// 1. Update���� transform�� ���� �̵� -> FixedUpdate���� rigidbody�� ���� �̵����� ���� physics frame������ ������Ʈ ��ġ Ȯ��
	// 2. rbCollider���� �������� �ٸ� �ݶ��̴����� �浹 üũ
	//   2.1. ���� �浹�� ��� ���ʿ� Enter(), ���浹�� ��� ���ʿ� Stay() ȣ��
	//   2.2. �ݶ��̴��� ���ŵ� bestSeparateAxis�� minOverlap���� �̿��� �о�� -> ������Ʈ ��ġ ����
	// 3. ���������� Exit üũ

	PhysicsPhase1_SettleObjectPosition(bodies);
	PhysicsPhase2_CheckCollision(bodies, rbColliders, allColliders);
}

void Scene::PhysicsPhase1_SettleObjectPosition(const std::vector<Rigidbody2D*>& bodies)
{
	for (auto body : bodies)
	{
		if (body->bodyType == BodyType::Dynamic)
		{
			body->velocity += Vector3{ 0, -PhysicsConstant::gravityForce * body->mass * body->gravityScale * Time::GetFixedDeltaTime(), 0 };
			body->MoveObject();

			// dragging
			body->velocity = body->velocity * (1 - (body->linear_drag * Time::GetFixedDeltaTime()));
		}
	}
}

// �� �浹 �˻� ���ึ�� ���� �浹 ���� ������Ʈ �迭�� �����ؾ� ��
// ���� �����ӱ��� �浹 ������Ʈ �迭�� ���ٰ� �̹� �����ӿ� �߰��� -> OnCollisionEnter
// ���� �����ӱ��� �浹 ������Ʈ �迭�� �ִٰ� �̹� �����ӿ� ������ -> OnCollisionExit
// ���� �����ӿ��� �浹 ������Ʈ �迭�� �־��� �̹����� ���� -> OnCollisionStay
// ���� ������Ʈ�� �����Ǿ� ������ �ݶ��̴��� ���� ���� �ϳ��� ����ұ� �ߴµ�, ����Ƽ������ ���� ó���Ѵ�. ����Ƽ�� �׷��� �� �� ���� ������ ���� ���̴� �̴�� ����.
void Scene::PhysicsPhase2_CheckCollision(const std::vector<Rigidbody2D*>& bodies, const std::unordered_set<Collider*>& rbColliders, const std::unordered_set<Collider*>& allColliders)
{
	// Enter, Stay, Exit üũ
	// ����Ƽó�� Rigidbody ������Ʈ�� ������ ������Ʈ�� �����ùķ��̼��� ����� ��
	for (Collider* rbCollider : rbColliders)
	{
		// �������� �浹, ������ allColliders ��Ͽ� ���� -> ������Ʈ�� ���ŵ� -> Exit, CollisionInfo�� null
		// <07-14> ������ �����ε� ����ߴ�... �ݺ��ڸ� ���� ��Ҹ� ����� �� �ݺ��ڴ� �翬�� �� ���� �ȴ�(Iterator Invalidation).
		for (auto iter = rbCollider->collided.begin(); iter != rbCollider->collided.end();)
		{
			if (allColliders.find(*iter) == allColliders.end())
			{
				iter = rbCollider->collided.erase(iter);
				Collision infoForRbObj(nullptr, nullptr);
				if (rbCollider->isTrigger == true)
				{
					rbCollider->GetOwner()->OnTriggerExitObject(infoForRbObj);
					// OnTrigger�� OnCollision���� ������Ʈ�� ���ŵ� �� ������, ������Ʈ�� ���Ŵ� �ǵ������� ���� �����ӿ��� �����ϰ� �Ͽ� ��� ��ũ��Ʈ ���� 1ȸ�� ��ȸ�ϵ��� �ϵ��� ����Ǿ� �����Ƿ� �˻縦 ���� �ʴ´�.
				}
				else
				{
					rbCollider->GetOwner()->OnCollisionExitObject(infoForRbObj);
				}
			}
			else
			{
				++iter;
			}
		}

		//���� rbCollider�� ��� �ݶ��̴� üũ
		for (auto other : allColliders)
		{
			if (other == rbCollider) continue;
			if (other->GetOwner() == rbCollider->GetOwner()) continue;
			// �±׷� �Ÿ���
			if (!rbCollider->ignore_collision_tags.empty() && !other->GetOwner()->tags.empty())
			{
				bool flag_skip = false;
				std::unordered_set<std::string>& tags = other->GetOwner()->GetTags();
				for (auto& tag : tags)
				{
					if (rbCollider->ignore_collision_tags.find(tag) != rbCollider->ignore_collision_tags.end())
					{
						flag_skip = true;
						break;
					}
				}
				if (flag_skip == true)
				{
					continue;
				}
			}



			// ���� �浹��
			if (rbCollider->IsCollideWith(other))
			{
				// �������� �浹�� -> Stay
				if (rbCollider->collided.find(other) != rbCollider->collided.end())
				{
					Collision infoForRbObj(other, other->GetOwner());
					Collision infoForOther(rbCollider, rbCollider->GetOwner());

					if (rbCollider->isTrigger == true || other->isTrigger == true)
					{
						rbCollider->GetOwner()->OnTriggerStayObject(infoForRbObj);
						// �������� �ǵ������� ���� �������� ���ŵǵ��� �����ϱ� �Ź� enable üũ�� ���� �ʴ´ٰ� ������, A ������Ʈ���� B ������Ʈ�� ����ٴ� �� B ������Ʈ�� �۵��� ��� ����ٴ� �ǹ̷� ������ �ʿ䰡 �ִ�. ���� �ٸ� ������Ʈ�� �Լ��� ȣ���� ���� enable üũ�� ������
						if (other->GetOwner()->GetParentEnable() == true)
							other->GetOwner()->OnTriggerStayObject(infoForOther);
					}
					else
					{
						rbCollider->GetOwner()->OnCollisionStayObject(infoForRbObj);
						if (other->GetOwner()->GetParentEnable() == true)
							other->GetOwner()->OnCollisionStayObject(infoForOther);

						// �о ���� ���ڸ��� other�� �����ߴٸ� ƨ�ܳ����� ���ƾ� �ϹǷ� other�� enable üũ�� �����ؾ� �Ѵ�.
						if (rbColliders.find(other) != rbColliders.end() && other->GetOwner()->GetParentEnable() == true)
						{
							Physics_RearrangePosition(rbCollider, other, true);
						}
						else if (other->GetOwner()->GetParentEnable() == true)
						{
							Physics_RearrangePosition(rbCollider, other, false);
						}
					}
				}
				// ó�� �浹 -> Enter
				else
				{
					rbCollider->collided.insert(other);
					Collision infoForRbObj(other, other->GetOwner());
					Collision infoForOther(rbCollider, rbCollider->GetOwner());

					if (rbCollider->isTrigger == true || other->isTrigger == true)
					{
						rbCollider->GetOwner()->OnTriggerEnterObject(infoForRbObj);
						if (other->GetOwner()->GetParentEnable() == true)
							other->GetOwner()->OnTriggerEnterObject(infoForOther);
					}
					else
					{
						rbCollider->GetOwner()->OnCollisionEnterObject(infoForRbObj);
						if (other->GetOwner()->GetParentEnable() == true)
							other->GetOwner()->OnCollisionEnterObject(infoForOther);

						if (rbColliders.find(other) != rbColliders.end() && other->GetOwner()->GetParentEnable() == true)
						{
							Physics_RearrangePosition(rbCollider, other, true);
						}
						else if (other->GetOwner()->GetParentEnable() == true)
						{
							Physics_RearrangePosition(rbCollider, other, false);
						}
					}
				}
			}
			// ���� �浹���� �ƴ� �� -> Exit
			else
			{
				// 3-1. ���������� �浹���̾��� ����
				if (rbCollider->collided.find(other) != rbCollider->collided.end())
				{
					rbCollider->collided.erase(other);
					Collision infoForRbObj(other, other->GetOwner());
					Collision infoForOther(rbCollider, rbCollider->GetOwner());
					if (rbCollider->isTrigger == true)
					{
						rbCollider->GetOwner()->OnTriggerExitObject(infoForRbObj);
						if (other->GetOwner()->GetParentEnable() == true)
							other->GetOwner()->OnTriggerExitObject(infoForOther);
					}
					else
					{
						rbCollider->GetOwner()->OnCollisionExitObject(infoForRbObj);
						if (other->GetOwner()->GetParentEnable() == true)
							other->GetOwner()->OnCollisionExitObject(infoForOther);
					}
				}
			}
		}
	}
}

void Scene::Physics_RearrangePosition(Collider* rbCollider, Collider* other, bool rearrange_other_too)
{
	// <07-13> 
	// �浹�� ���� ������ �ݿ����� �� �ϹǷ� Reflect���� �ʰ� ���⺤���� �ݴ�� �з�����. incident face��� �ϴ��� ���� ������ ����� ���ϳ�����. ���߿� �߰�����.
	// https://kareus.tistory.com/15
	// <07-14> �ӽ÷� �׳� -bestSeparateAxis�� �浹����� �������ͷ� ��Ƽ� Reflect �غôµ� �� �۵��ϳ�...

	// PhysicsPhase2���� ȣ��Ǵ� IsCollideWith() �ȿ��� bestSeparateAxis�� minOverlap�� ���ŵǴ� ���� ����� / bestSeparateAxis�� ���⵵ �о �������� ���ĵǾ� ����.
	// �浹 ���¸� �����ؾ� �ϹǷ�, epsilon ��ŭ ���ܳ��� �δ�.
	const float epsilon = DG::CollisionCheck::OVERLAP_EPSILON;
	Rigidbody2D* rb = rbCollider->GetOwner()->GetComponent<Rigidbody2D>();
	Rigidbody2D* otherRb = nullptr;
	if (rearrange_other_too == true)
	{
		otherRb = other->GetOwner()->GetComponent<Rigidbody2D>();
		if (otherRb == nullptr)
		{
			Assert(false, "failed to find otherRb");
			return;
		}
	}

	// <07-14>
	// �浹�� ��뵵 Rigidbody ������Ʈ�� �浹 �� ��ġ�� ���ؾ� �� (���� üũ���� ������Ʈ�� ������� ��ȸ�ϸ� �о�� ����)
	// �ٵ� �̷��� 3�� �̻��� ���ÿ� �浹�ϴ� ��Ȳ�� ó���� �� �Ѵ�
	// SAT �浹üũ �� �� otherCollider���� separate axis�� �ݴ� �������� �˾Ƽ� �����ϹǷ� ������ �Ȱ��� ���� ��.

	// overlapAmount�� epsilon���� ������ ���� ����
	if (rbCollider->minOverlap <= epsilon) return;

	// ������ bestSeparateAxis�� �浹 ǥ���� �������ͷ� ����� ���� Reflect�� ����
	// �ӷ��� ź�� �浹 ���� * resititution_coefficient
	if (rearrange_other_too == true)
	{
		Vector3 rbDirectionBackup = rb->velocity;
		rb->velocity = Physics::GetVelocityAfterCollision(rb->velocity, rb->mass, rb->restitution_coefficient, otherRb->velocity, otherRb->mass, rbCollider->bestSeparateAxis);
		otherRb->velocity = Physics::GetVelocityAfterCollision(otherRb->velocity, otherRb->mass, otherRb->restitution_coefficient, rbDirectionBackup, rb->mass, other->bestSeparateAxis);
	}
	else if (rearrange_other_too == false)
	{
		rb->velocity = Physics::GetVelocityAfterCollision(rb->velocity, rb->mass, rb->restitution_coefficient, rbCollider->bestSeparateAxis);
	}

	if (Math::Approximate(rb->restitution_coefficient, 0.0f))
	{
		// �״�� ���� �� / �浹 ���¸� �����ؾ� �ϹǷ� epsilon��ŭ �� �δ�.
		rb->GetOwner()->transform->Translate(rbCollider->bestSeparateAxis * (rbCollider->minOverlap - epsilon));
	}
	else // ƨ�ܳ��� ��
	{
		// �ϴ� overlap�� ��ŭ separateAxis �������� �о��, �� ������ rb->velocity �������� overlap * rb->restitution_coefficient ��ŭ �̵���Ŵ (���⼭ �ݹ߰�� �ܼ� ���ϴ� �� �� �̻��ѵ�?)
		rb->GetOwner()->transform->Translate(rbCollider->bestSeparateAxis * rbCollider->minOverlap);
		rb->GetOwner()->transform->Translate(rb->velocity.Normalize() * rbCollider->minOverlap * rb->restitution_coefficient);
	}

	if (rearrange_other_too == true)
	{
		if (Math::Approximate(otherRb->restitution_coefficient, 0.0f))
		{
			otherRb->GetOwner()->transform->Translate(other->bestSeparateAxis * (other->minOverlap - epsilon));
		}
		else
		{
			otherRb->GetOwner()->transform->Translate(other->bestSeparateAxis * other->minOverlap);
			otherRb->GetOwner()->transform->Translate(otherRb->velocity.Normalize() * other->minOverlap * otherRb->restitution_coefficient);
		}
	}



	// obsolete
	//if (Math::Approximate(moveDir.x, 0.0f) && Math::Approximate(moveDir.y, 0.0f))
	//{
	//	// ���Ʒ����� �浹�� ���
	//	if (overlap.y <= overlap.x && overlap.y > epsilon)
	//	{
	//		// �Ʒ����� �浹
	//		if (rbCenter.y > otherCenter.y)
	//		{
	//			rb->velocity.y = -rb->velocity.y * rb->bottomElasticity;
	//			rb->GetOwner()->transform->Translate(0, (overlap.y - epsilon), 0);
	//		}
	//		// ������ �浹
	//		else if (rbCenter.y <= otherCenter.y)
	//		{
	//			rb->velocity.y = -rb->velocity.y * rb->topElasticity;
	//			rb->GetOwner()->transform->Translate(0, -(overlap.y - epsilon), 0);
	//		}
	//	}
	//	// �¿쿡�� �浹�� ���
	//	else if (overlap.y > overlap.x && overlap.x > epsilon)
	//	{
	//		// �¿��� �浹
	//		if (rbCenter.x > otherCenter.x)
	//		{
	//			rb->velocity.x = -rb->velocity.x * rb->sideElasticity;
	//			rb->GetOwner()->transform->Translate((overlap.x - epsilon), 0, 0);
	//		}
	//		// �쿡�� �浹
	//		else if (rbCenter.x <= otherCenter.x)
	//		{
	//			rb->velocity.x = -rb->velocity.x * rb->sideElasticity;
	//			rb->GetOwner()->transform->Translate(-(overlap.x - epsilon), 0, 0);
	//		}
	//	}
	//}
}





void Scene::LateUpdate()
{
	// Update �������� SetDrawingPriority()�� ���� set �����̳� �� �������� �Ͼ ��� set/map iterator incompatible ���� �߻� / ���纻���� ������.
	auto objectsCopy = objects;
	for (auto iter = objectsCopy.begin(); iter != objectsCopy.end();)
	{
		Object* curObj = (*iter);
		++iter; // ������Ʈ �������� ������ ���� ��츦 �����Ͽ� �ݺ��ڸ� �̸� ����

		if (curObj->GetParentEnable() == false || curObj->GetStarted() == false) continue;
		curObj->LateUpdateObject();
	}
}



void Scene::Draw()
{
	// std::set �ڷᱸ���� ����ϴ� objects�� drawingPriority�� �������� ���ĵ� ���¸� �����Ѵ�.

	std::vector<Object*> UIElements;
	for (auto curObj : objects)
	{
		if (curObj->GetParentEnable() == false || curObj->started == false) continue;

		if (curObj->GetObjectLayer() == ObjectLayer::UI)
		{
			UIElements.push_back(curObj);
			continue;
		}

		SpriteRenderer* renderer = curObj->GetComponent<SpriteRenderer>();
		if (renderer != nullptr && renderer->enable == true) renderer->Draw();
		TextRenderer* tRenderer = curObj->GetComponent<TextRenderer>();
		if (tRenderer != nullptr && tRenderer->enable == true) tRenderer->Draw();
		curObj->Draw();
	}
	for (auto curObj : UIElements)
	{
		SpriteRenderer* renderer = curObj->GetComponent<SpriteRenderer>();
		if (renderer != nullptr && renderer->enable == true) renderer->Draw();
		TextRenderer* tRenderer = curObj->GetComponent<TextRenderer>();
		if (tRenderer != nullptr && tRenderer->enable == true) tRenderer->Draw();
		curObj->Draw();
	}
}