#include "Transform.h"
#include "Math.h"
#include "Object.h"
#include "Time.h"

Transform::~Transform()
{
	SetParent(nullptr);
	for (auto c : child)
	{
		c->parent = nullptr;
	}
}
Vector3 Transform::GetPosition() const
{
	if (parent == nullptr)
	{
		return position;
	}
	else
	{
		// <24-07-01>
		// GetPosition은 그냥 월드좌표 얻는 용도로 쓰는 함수
		// 실제로 그릴 땐 GetLocalPosition()으로 받아서 재귀적으로 곱하며 얻어온 부모의 Matrix에 로컬값을 곱하는 방식을 쓴다.
		Vector3 scaledPosition = position;
		scaledPosition.x *= parent->GetScale().x;
		scaledPosition.y *= parent->GetScale().y;
		Vector3 rotatedPosition = scaledPosition.Rotate(Math::Deg2Rad(parent->GetRotation().z));
		Vector3 relativePosition = parent->GetPosition() + rotatedPosition;
		return relativePosition;
	}
}
Vector3 Transform::GetLocalPosition() const
{
	return position;
}
void Transform::SetPosition(Vector3 vec)
{
	position = vec;
}
void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}
void Transform::SetPositionX(float x)
{
	position.x = x;
}
void Transform::SetPositionY(float y)
{
	position.y = y;
}
void Transform::SetPositionZ(float z)
{
	position.z = z;
}
Vector3 Transform::GetRotation() const
{
	Vector3 _rotation = rotation;
	_rotation.x = fmodf(_rotation.x, 360.f);
	_rotation.y = fmodf(_rotation.y, 360.f);
	_rotation.z = fmodf(_rotation.z, 360.f);

	if (parent == nullptr)
	{
		return _rotation;
	}
	else
	{
		return parent->GetRotation() + _rotation;
	}
}
Vector3 Transform::GetLocalRotation() const
{
	return rotation;
}
void Transform::SetRotation(Vector3 vec)
{
	rotation = vec;
}
void Transform::SetRotation(float x, float y, float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}
void Transform::SetRotationX(float x)
{
	rotation.x = x;
}
void Transform::SetRotationY(float y)
{
	rotation.y = y;
}
void Transform::SetRotationZ(float z)
{
	rotation.z = z;
}
Vector3 Transform::GetScale() const
{
	if (parent == nullptr)
	{
		return scale;
	}
	else
	{
		Vector3 relativeScale = parent->GetScale() * scale;
		return relativeScale;
	}
}
Vector3 Transform::GetLocalScale() const
{
	return scale;
}
void Transform::SetScale(Vector3 vec)
{
	scale = vec;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void Transform::SetScaleX(float x)
{
	scale.x = x;
}
void Transform::SetScaleY(float y)
{
	scale.y = y;
}
void Transform::SetScaleZ(float z)
{
	scale.z = z;
}

void Transform::LookAt(Transform* other)
{
	Vector3 dPos = other->GetPosition() - GetPosition();
	float degree = Math::Rad2Deg(-atan2f(dPos.y, dPos.x));
	rotation.z = degree;
}
void Transform::LookAt(Vector3 dPos)
{
	dPos = dPos - GetPosition();
	float degree = Math::Rad2Deg(-atan2f(dPos.y, dPos.x));
	rotation.z = degree;
}

Transform* Transform::GetParent() const
{
	return parent;
}
void Transform::SetParent(Transform* _parent)
{
	MakeRelationship(_parent, this);
}
std::unordered_set<Transform*>* Transform::GetChild()
{
	return &child;
}
void Transform::SetChild(Transform* _child)
{
	MakeRelationship(this, _child);
}

void Transform::MakeRelationship(Transform* _parent, Transform* _child)
{
	//Vector3 worldPosition = _child->GetPosition();
	if (_parent == nullptr)
	{
		if (_child->parent != nullptr)
		{
			_child->parent->child.erase(_child);
		}
		_child->parent = nullptr;
		return;
	}
	if (_child == nullptr)
	{
		return;
	}

	if (_child->parent != nullptr) // 자식이 이미 다른 부모를 가졌던 경우
	{
		_child->parent->child.erase(_child);
	}
	_child->parent = _parent;
	_parent->child.insert(_child);
	//_child->SetPosition(worldPosition);
}



void Transform::Translate(Vector3 vec)
{
	Translate(vec.x, vec.y, vec.z);
}
void Transform::Translate(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
}
void Transform::Rotate(Vector3 vec)
{
	Rotate(vec.x, vec.y, vec.z);
}
void Transform::Rotate(float x, float y, float z)
{
	rotation.x += x;
	rotation.y += y;
	rotation.z += z;
}
void Transform::ScaleMult(Vector3 vec)
{
	ScaleMult(vec.x, vec.y, vec.z);
}
void Transform::ScaleMult(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}
void Transform::ScaleAdd(Vector3 vec)
{
	ScaleAdd(vec.x, vec.y, vec.z);
}
void Transform::ScaleAdd(float x, float y, float z)
{
	scale.x += x;
	scale.y += y;
	scale.z += z;
}


Vector3 Transform::Up()
{
	return Vector3::Up().Rotate(Math::Deg2Rad(rotation.z)).Normalize();
}
Vector3 Transform::Right()
{
	return Vector3::Right().Rotate(Math::Deg2Rad(rotation.z)).Normalize();
}
Vector3 Transform::Down()
{
	return Vector3::Down().Rotate(Math::Deg2Rad(rotation.z)).Normalize();
}
Vector3 Transform::Left()
{
	return Vector3::Left().Rotate(Math::Deg2Rad(rotation.z)).Normalize();
}



void Transform::EasyMove_MoveTo(Vector3 destination, float duration)
{
	Vector3 movement = destination - position;
	EasyMove_MoveBy(movement, duration);
}
void Transform::EasyMove_MoveBy(Vector3 movement, float duration)
{
	Object* obj = GetOwner();
	Vector3 movePerSec = movement / duration;

	obj->AddTimerFunction([=](bool lastUpdate) mutable
		{
			// 람다는 기본적으로 외부 변수를 const로 캡쳐해온다. 그래서 mutable 안 써주면 Vector3 연산자 오버로딩 이용 못 함(non-const 함수기 때문에)
			// mutable은 기본적으로 외부 변수를 람다식 내에서 수정할 수 있게 하는 녀석(const로 캡쳐하지 않는다는 의미인 듯). 단, 그렇다고 외부변수를 직접 수정하는 건 아니고 람다식 내부에서 새로 같은 이름의 변수를 만들어서 수정한다. 외부 변수를 수정하려면 주소로 가져와야 함
			float dt = Time::GetDeltaTime();
			obj->transform->Translate(movePerSec * dt);
		}, true, duration, 1);
}
void Transform::EasyMove_RotateTo(Vector3 _rotation, float duration)
{
	Vector3 rt = _rotation - rotation;
	EasyMove_RotateBy(rt, duration);
}
void Transform::EasyMove_RotateBy(Vector3 _rotation, float duration)
{
	Object* obj = GetOwner();
	Vector3 rotatePerSec = _rotation / duration;

	obj->AddTimerFunction([=](bool lastUpdate) mutable
		{
			float dt = Time::GetDeltaTime();
			obj->transform->Rotate(rotatePerSec * dt);
		}, true, duration, 1);
}
void Transform::EasyMove_ScaleTo(Vector3 _scale, float duration)
{
	Vector3 sc = _scale - scale;
	EasyMove_ScaleBy(sc, duration);
}
void Transform::EasyMove_ScaleBy(Vector3 _scale, float duration)
{
	Object* obj = GetOwner();
	Vector3 scalePerSec = _scale / duration;

	obj->AddTimerFunction([=](bool lastUpdate) mutable
		{
			float dt = Time::GetDeltaTime();
			obj->transform->ScaleAdd(scalePerSec * dt);
		}, true, duration, 1);
}

// TimerFunctionManager.h 파일에도 적어놨지만, function으로 객체화하여 관리하는 람다식은 function이 새로 생성되지 않는 한 내부의 변수도 새로 생성되지 않는다. 즉, 람다식에서 값으로 캡쳐한 변수들은 사실상 함수 내부의 static변수처럼 변경내용을 유지하게 된다.
void Transform::EasyMove_ShakeHorizontal(float magnitude, float duration, int frequency, bool startToRight, EasyMove_Type type, bool restore_transform_when_done)
{
	Object* obj = GetOwner();

	Vector3 initPosition;
	if (restore_transform_when_done == true) initPosition = obj->transform->GetPosition();

	const float init_magnitude = magnitude;
	float radian = 0;

	if (frequency <= 0)
	{
		frequency = duration / 0.05f;
	}

	const float radPerSecond = Math::PI * frequency;
	const float maxRad = radPerSecond * duration;

	float dir;
	if (startToRight) dir = 1.f;
	else dir = -1.f;

	obj->AddTimerFunction([=](bool lastUpdate) mutable
		{
			if (lastUpdate == true && restore_transform_when_done == true)
			{
				obj->transform->SetPosition(initPosition);
			}
			else
			{
				if (type == EasyMove_Type::LinearShrink)
				{
					magnitude = init_magnitude * (maxRad - radian) / maxRad;
				}
				else if (type == EasyMove_Type::LinearGrow)
				{
					magnitude = init_magnitude * (2 * (radian / maxRad));
				}

				float dt = Time::GetDeltaTime();
				obj->transform->Translate(std::cos(radian) * magnitude * dt * dir, 0, 0);
				radian += radPerSecond * dt;
			}
		}, true, duration, 1);
}
void Transform::EasyMove_ShakeVertical(float magnitude, float duration, int frequency, bool startToDown, EasyMove_Type type, bool restore_transform_when_done)
{
	Object* obj = GetOwner();

	Vector3 initPosition;
	if (restore_transform_when_done == true) initPosition = obj->transform->GetPosition();


	const float init_magnitude = magnitude;
	float radian = 0;

	if (frequency <= 0)
	{
		frequency = duration / 0.05f;
	}

	const float radPerSecond = Math::PI * frequency;
	const float maxRad = radPerSecond * duration;

	float dir;
	if (startToDown) dir = 1.f;
	else dir = -1.f;

	obj->AddTimerFunction([=](bool lastUpdate) mutable
		{
			if (lastUpdate == true && restore_transform_when_done == true)
			{
				obj->transform->SetPosition(initPosition);
			}
			else
			{
				if (type == EasyMove_Type::LinearShrink)
				{
					magnitude = init_magnitude * (maxRad - radian) / maxRad;
				}
				else if (type == EasyMove_Type::LinearGrow)
				{
					magnitude = init_magnitude * (2 * (radian / maxRad));
				}

				float dt = Time::GetDeltaTime();
				obj->transform->Translate(0, std::cos(radian) * magnitude * dt * dir, 0);
				radian += radPerSecond * dt;
			}
		}, true, duration, 1);
}
void Transform::EasyMove_ShakeScale(float magnitude, float duration, int frequency, bool startToUpscale, EasyMove_Type type, bool restore_transform_when_done)
{
	Object* obj = GetOwner();

	Vector3 initScale;
	if (restore_transform_when_done == true) initScale = obj->transform->GetScale();


	const float init_magnitude = magnitude;
	float radian = 0;

	if (frequency <= 0)
	{
		frequency = duration / 0.05f;
	}

	const float radPerSecond = Math::PI * frequency;
	const float maxRad = radPerSecond * duration;

	float dir;
	if (startToUpscale) dir = 1.f;
	else dir = -1.f;

	obj->AddTimerFunction([=](bool lastUpdate) mutable
		{
			if (lastUpdate == true && restore_transform_when_done == true)
			{
				obj->transform->SetScale(initScale);
			}
			else
			{
				if (type == EasyMove_Type::LinearShrink)
				{
					magnitude = init_magnitude * (maxRad - radian) / maxRad;
				}
				else if (type == EasyMove_Type::LinearGrow)
				{
					magnitude = init_magnitude * (2 * (radian / maxRad));
				}

				float dt = Time::GetDeltaTime();
				obj->transform->ScaleAdd(std::cos(radian) * magnitude * dt * dir, std::cos(radian) * magnitude * dt * dir, 0);
				radian += radPerSecond * dt;
			}
		}, true, duration, 1);
}