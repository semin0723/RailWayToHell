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
	
	// set은 find를 할 때도 정렬 기준 함수를 사용하기 때문에 먼저 priority를 바꾼 다음 찾으면 못 찾는다
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
	if (iter == objectsByName.end()) return false; // 오브젝트를 찾지 못 함

	// std::string을 find에 그대로 넣어서 찾는 건 불가능? find(name)으로 하면 동작을 안 한다.
	auto iter2 = objectsByName.find(name.c_str());
	if (iter == objectsByName.end()) return false; // 중복된 이름으로 변경 시도

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

		// Initialize 이후 메인카메라가 설정되지 않은 경우, 우선 오브젝트 중에 카메라 컴포넌트가 있는지 찾고, 그래도 없다면 추가한다.
		if (mainCamera == nullptr)
		{
			Debug::Log("카메라를 찾을 수 없음.");
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


		// 여기서 생성된 오브젝트의 Start가 호출이 안 되면 Draw() -> Update() 순이라 또 문제 생긴다
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
	// Update 과정에서 SetDrawingPriority()에 의해 set 컨테이너 내 재정렬이 일어날 경우 set/map iterator incompatible 오류 발생 / 복사본으로 돌린다.
	auto objectsCopy = objects;
	for (auto iter = objectsCopy.cbegin(); iter != objectsCopy.cend();)
	{
		Object* curObj = (*iter);
		++iter; // 업데이트 과정에서 문제가 생길 경우를 예방하여 반복자만 미리 갱신

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

	// 충돌체크는 FixedUpdate에서 진행
}



// 충돌 체크는 나중에 physics thread와 physics loop를 구현하여 그 쪽으로 뺄 것
void Scene::FixedUpdate()
{
	std::vector<Rigidbody2D*> bodies;
	std::unordered_set<Collider*> rbColliders; // Rigidbody가 부착된 오브젝트의 콜라이더
	std::unordered_set<Collider*> allColliders; // 모든 콜라이더

	// Update 과정에서 SetDrawingPriority()에 의해 set 컨테이너 내 재정렬이 일어날 경우 set/map iterator incompatible 오류 발생 / 복사본으로 돌린다.
	auto objectsCopy = objects;
	for (auto iter = objectsCopy.cbegin(); iter != objectsCopy.cend();)
	{
		Object* curObj = (*iter);
		++iter; // 업데이트 과정에서 문제가 생길 경우를 예방하여 반복자만 미리 갱신

		if (curObj->GetParentEnable() == false || curObj->started == false) continue;
		curObj->FixedUpdateObject(); // 초기화는 Update에서만

		// ObjectLayer::Object 오브젝트들은 잠재적 물리 시뮬레이션 적용 대상
		if (curObj->GetObjectLayer() == ObjectLayer::Object)
		{
			// 무효 콜라이더는 알아서 걸러져서 온다.
			std::vector<Collider*> _colliders = curObj->GetComponents<Collider>();
			if (!_colliders.empty())
			{
				// std::inserter는 자료구조의 insert() 함수를 호출한다.
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
	// 1. Update에서 transform을 통한 이동 -> FixedUpdate에서 rigidbody를 통한 이동으로 현재 physics frame에서의 오브젝트 위치 확정
	// 2. rbCollider들을 기준으로 다른 콜라이더와의 충돌 체크
	//   2.1. 최초 충돌인 경우 양쪽에 Enter(), 재충돌인 경우 양쪽에 Stay() 호출
	//   2.2. 콜라이더에 갱신된 bestSeparateAxis와 minOverlap값을 이용해 밀어내기 -> 오브젝트 위치 조정
	// 3. 마지막으로 Exit 체크

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

// 매 충돌 검사 수행마다 현재 충돌 중인 컴포넌트 배열을 갱신해야 함
// 이전 프레임까지 충돌 컴포넌트 배열에 없다가 이번 프레임에 추가됨 -> OnCollisionEnter
// 이전 프레임까지 충돌 컴포넌트 배열에 있다가 이번 프레임에 없어짐 -> OnCollisionExit
// 이전 프레임에도 충돌 컴포넌트 배열에 있었고 이번에도 있음 -> OnCollisionStay
// 같은 오브젝트에 부착되어 있으면 콜라이더가 여러 개라도 하나로 취급할까 했는데, 유니티에서도 따로 처리한다. 유니티가 그렇게 한 건 뭔가 이유가 있을 것이니 이대로 놔둠.
void Scene::PhysicsPhase2_CheckCollision(const std::vector<Rigidbody2D*>& bodies, const std::unordered_set<Collider*>& rbColliders, const std::unordered_set<Collider*>& allColliders)
{
	// Enter, Stay, Exit 체크
	// 유니티처럼 Rigidbody 컴포넌트가 부착된 오브젝트만 물리시뮬레이션의 대상이 됨
	for (Collider* rbCollider : rbColliders)
	{
		// 이전까지 충돌, 지금은 allColliders 목록에 없음 -> 오브젝트가 제거됨 -> Exit, CollisionInfo는 null
		// <07-14> 간단한 문제인데 고생했다... 반복자를 통해 요소를 지우면 그 반복자는 당연히 못 쓰게 된다(Iterator Invalidation).
		for (auto iter = rbCollider->collided.begin(); iter != rbCollider->collided.end();)
		{
			if (allColliders.find(*iter) == allColliders.end())
			{
				iter = rbCollider->collided.erase(iter);
				Collision infoForRbObj(nullptr, nullptr);
				if (rbCollider->isTrigger == true)
				{
					rbCollider->GetOwner()->OnTriggerExitObject(infoForRbObj);
					// OnTrigger나 OnCollision에서 오브젝트가 제거될 수 있지만, 오브젝트의 제거는 의도적으로 다음 프레임에서 제거하게 하여 모든 스크립트 등을 1회는 순회하도록 하도록 설계되어 있으므로 검사를 하지 않는다.
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

		//현재 rbCollider와 모든 콜라이더 체크
		for (auto other : allColliders)
		{
			if (other == rbCollider) continue;
			if (other->GetOwner() == rbCollider->GetOwner()) continue;
			// 태그로 거르기
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



			// 현재 충돌중
			if (rbCollider->IsCollideWith(other))
			{
				// 이전에도 충돌중 -> Stay
				if (rbCollider->collided.find(other) != rbCollider->collided.end())
				{
					Collision infoForRbObj(other, other->GetOwner());
					Collision infoForOther(rbCollider, rbCollider->GetOwner());

					if (rbCollider->isTrigger == true || other->isTrigger == true)
					{
						rbCollider->GetOwner()->OnTriggerStayObject(infoForRbObj);
						// 위에서는 의도적으로 다음 루프에서 제거되도록 했으니까 매번 enable 체크를 하지 않는다고 했지만, A 오브젝트에서 B 오브젝트를 지운다는 건 B 오브젝트의 작동을 즉시 멈춘다는 의미로 이해할 필요가 있다. 따라서 다른 오브젝트의 함수를 호출할 때는 enable 체크를 진행함
						if (other->GetOwner()->GetParentEnable() == true)
							other->GetOwner()->OnTriggerStayObject(infoForOther);
					}
					else
					{
						rbCollider->GetOwner()->OnCollisionStayObject(infoForRbObj);
						if (other->GetOwner()->GetParentEnable() == true)
							other->GetOwner()->OnCollisionStayObject(infoForOther);

						// 밀어낼 때도 닿자마자 other를 제거했다면 튕겨나가지 말아야 하므로 other의 enable 체크를 진행해야 한다.
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
				// 처음 충돌 -> Enter
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
			// 현재 충돌중이 아닐 때 -> Exit
			else
			{
				// 3-1. 이전까지는 충돌중이었을 때만
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
	// 충돌한 면의 각도를 반영하지 못 하므로 Reflect되지 않고 방향벡터의 반대로 밀려난다. incident face라고 하던데 대충 내적을 사용해 구하나보다. 나중에 추가하자.
	// https://kareus.tistory.com/15
	// <07-14> 임시로 그냥 -bestSeparateAxis를 충돌평면의 법선벡터로 삼아서 Reflect 해봤는데 잘 작동하네...

	// PhysicsPhase2에서 호출되는 IsCollideWith() 안에서 bestSeparateAxis와 minOverlap이 갱신되는 것이 보장됨 / bestSeparateAxis의 방향도 밀어낼 방향으로 정렬되어 담긴다.
	// 충돌 상태를 유지해야 하므로, epsilon 만큼 남겨놓고 민다.
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
	// 충돌한 상대도 Rigidbody 오브젝트면 충돌 후 조치를 취해야 함 (물리 체크에서 오브젝트를 순서대로 순회하며 밀어내기 때문)
	// 근데 이래도 3개 이상이 동시에 충돌하는 상황은 처리를 못 한다
	// SAT 충돌체크 할 때 otherCollider에는 separate axis를 반대 방향으로 알아서 저장하므로 로직은 똑같이 쓰면 됨.

	// overlapAmount가 epsilon보다 작으면 현상 유지
	if (rbCollider->minOverlap <= epsilon) return;

	// 방향은 bestSeparateAxis를 충돌 표면의 법선벡터로 삼았을 때의 Reflect된 벡터
	// 속력은 탄성 충돌 공식 * resititution_coefficient
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
		// 그대로 멈출 때 / 충돌 상태를 유지해야 하므로 epsilon만큼 덜 민다.
		rb->GetOwner()->transform->Translate(rbCollider->bestSeparateAxis * (rbCollider->minOverlap - epsilon));
	}
	else // 튕겨나갈 때
	{
		// 일단 overlap된 만큼 separateAxis 방향으로 밀어내고, 그 다음에 rb->velocity 방향으로 overlap * rb->restitution_coefficient 만큼 이동시킴 (여기서 반발계수 단순 곱하는 건 좀 이상한데?)
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
	//	// 위아래에서 충돌한 경우
	//	if (overlap.y <= overlap.x && overlap.y > epsilon)
	//	{
	//		// 아래에서 충돌
	//		if (rbCenter.y > otherCenter.y)
	//		{
	//			rb->velocity.y = -rb->velocity.y * rb->bottomElasticity;
	//			rb->GetOwner()->transform->Translate(0, (overlap.y - epsilon), 0);
	//		}
	//		// 위에서 충돌
	//		else if (rbCenter.y <= otherCenter.y)
	//		{
	//			rb->velocity.y = -rb->velocity.y * rb->topElasticity;
	//			rb->GetOwner()->transform->Translate(0, -(overlap.y - epsilon), 0);
	//		}
	//	}
	//	// 좌우에서 충돌한 경우
	//	else if (overlap.y > overlap.x && overlap.x > epsilon)
	//	{
	//		// 좌에서 충돌
	//		if (rbCenter.x > otherCenter.x)
	//		{
	//			rb->velocity.x = -rb->velocity.x * rb->sideElasticity;
	//			rb->GetOwner()->transform->Translate((overlap.x - epsilon), 0, 0);
	//		}
	//		// 우에서 충돌
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
	// Update 과정에서 SetDrawingPriority()에 의해 set 컨테이너 내 재정렬이 일어날 경우 set/map iterator incompatible 오류 발생 / 복사본으로 돌린다.
	auto objectsCopy = objects;
	for (auto iter = objectsCopy.begin(); iter != objectsCopy.end();)
	{
		Object* curObj = (*iter);
		++iter; // 업데이트 과정에서 문제가 생길 경우를 예방하여 반복자만 미리 갱신

		if (curObj->GetParentEnable() == false || curObj->GetStarted() == false) continue;
		curObj->LateUpdateObject();
	}
}



void Scene::Draw()
{
	// std::set 자료구조를 사용하는 objects는 drawingPriority를 기준으로 정렬된 상태를 보장한다.

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