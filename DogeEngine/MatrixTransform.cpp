#include "MatrixTransform.h"
#include "DirectXFramework.h"
#include "StandardFramework.h"
#include "Transform.h"
#include "Camera.h"
#include "Core.h"
#include "Object.h"

D2D1::Matrix3x2F MatrixTransform::GetWorldTransformMatrix(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	D2D1::Matrix3x2F worldMatrix =
		D2D1::Matrix3x2F::Scale(scale.x, scale.y) *
		D2D1::Matrix3x2F::Rotation(rotation.z) *
		D2D1::Matrix3x2F::Translation(position.x, -position.y);

	return worldMatrix;
}

D2D1::Matrix3x2F MatrixTransform::GetWorldTransformMatrix(const Transform* transform)
{
	if (transform->GetParent() == nullptr)
	{
		return GetWorldTransformMatrix(transform->GetLocalPosition(), transform->GetLocalRotation(), transform->GetLocalScale());
	}

	D2D1::Matrix3x2F parentMatrix = GetWorldTransformMatrix(transform->GetParent());

	D2D1::Matrix3x2F myMatrix = GetWorldTransformMatrix(transform->GetLocalPosition(), transform->GetLocalRotation(), transform->GetLocalScale());

	// �ڽ� * �θ� ������ ���ؾ� ��. ���� ��ǥ�迡 ���� ����� ���ϵ���, �ϼ��� �ڽ� ����� �θ� ��� ���������� ����� ��ȯ�� ���ϴ� ���̱� �����̴�.
	return myMatrix * parentMatrix;
}

D2D1::Matrix3x2F MatrixTransform::GetCameraInverseTransformMatrix(const Camera* camera)
{
	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return D2D1::Matrix3x2F{};
		}
	}

	Transform* cameraTransform = camera->GetOwner()->transform;

	// ī�޶� ������ ������Ʈ�� �ڽ� ������Ʈ�� ���, �켱 �θ��� ���� ����� ���� �� ���Ѵ�.
	D2D1::Matrix3x2F worldMatrix = D2D1::Matrix3x2F::Identity();
	if (cameraTransform->GetParent() != nullptr)
	{
		worldMatrix = GetWorldTransformMatrix(cameraTransform->GetParent());
	}

	// ī�޶��� ���, ��ȯ ������ ������ ī�޶� ������ ������Ʈ�� �������� �ƴ϶�, ī�޶��� ���� ��� ������(����Ʈ ��ǥ (0, 0))�� �Ǿ�� �Ѵ�.
	// ���� Translation�� ī�޶��� �»�� ���������� �̵�������, Scale, Rotation�� ī�޶��� ������ �������� ����Ǿ�� �Ѵ�.
	Vector3 position = cameraTransform->GetLocalPosition();
	Vector2 camLeftTopAnchor{ position.x - camera->width / 2, -position.y - camera->height / 2 };
	worldMatrix = worldMatrix *
		D2D1::Matrix3x2F::Scale(camera->scale, camera->scale, D2D1::Point2F(camera->width / 2, camera->height / 2)) *
		D2D1::Matrix3x2F::Rotation(camera->rotation, D2D1::Point2F(camera->width / 2, camera->height / 2)) *
		D2D1::Matrix3x2F::Translation(camLeftTopAnchor.x, camLeftTopAnchor.y);
	worldMatrix.Invert();

	return worldMatrix;
}

D2D1::Matrix3x2F MatrixTransform::GetCameraToViewTransformMatrix(const Camera* camera)
{
	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return D2D1::Matrix3x2F{};
		}
	}

	D2D1::Matrix3x2F cameraInverse = D2D1::Matrix3x2F::Translation(-camera->width / 2.f, -camera->height / 2.f);
	cameraInverse.Invert();

	return cameraInverse;
}






void MatrixTransform::ClearMatrix(ID2D1DeviceContext* deviceContext)
{
	deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
}






void MatrixTransform::SetWorldTransformMatrix(ID2D1DeviceContext* deviceContext, const Transform* transform, const Camera* camera)
{
	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return;
		}
	}

	D2D1::Matrix3x2F worldMatrix = GetWorldTransformMatrix(transform) * GetCameraInverseTransformMatrix(camera);

	deviceContext->SetTransform(worldMatrix);
}

void MatrixTransform::SetWorldTransformMatrix(ID2D1DeviceContext* deviceContext, const Vector3& position, const Vector3& rotation, const Vector3& scale, const Camera* camera)
{
	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return;
		}
	}

	D2D1::Matrix3x2F worldMatrix = GetWorldTransformMatrix(position, rotation, scale) * GetCameraInverseTransformMatrix(camera);

	deviceContext->SetTransform(worldMatrix);
}

void MatrixTransform::SetUITransformMatrix(ID2D1DeviceContext* deviceContext, const Transform* transform, const Camera* camera)
{
	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return;
		}
	}

	D2D1::Matrix3x2F cameraInverse = GetCameraToViewTransformMatrix(camera);
	D2D1::Matrix3x2F UIMatrix = GetWorldTransformMatrix(transform) * cameraInverse;

	deviceContext->SetTransform(UIMatrix);
}

void MatrixTransform::SetUITransformMatrix(ID2D1DeviceContext* deviceContext, const Vector3& position, const Vector3& rotation, const Vector3& scale, const Camera* camera)
{
	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return;
		}
	}

	D2D1::Matrix3x2F cameraInverse = GetCameraToViewTransformMatrix(camera);
	D2D1::Matrix3x2F UIMatrix = GetWorldTransformMatrix(position, rotation, scale) * cameraInverse;

	deviceContext->SetTransform(UIMatrix);
}






void MatrixTransform::SetWorldTransformMatrixWithAdditionalScaling(ID2D1DeviceContext* deviceContext, const Transform* transform, const Vector2 additionalScaling, const Camera* camera)
{
	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return;
		}
	}

	D2D1::Matrix3x2F parentMatrix = D2D1::Matrix3x2F::Identity();
	if (transform->GetParent() != nullptr)
	{
		parentMatrix = parentMatrix * GetWorldTransformMatrix(transform->GetParent());
	}

	Vector3 scale = transform->GetLocalScale();
	scale.x = scale.x * additionalScaling.x;
	scale.y = scale.y * additionalScaling.y;
	D2D1::Matrix3x2F myMatrix = GetWorldTransformMatrix(transform->GetLocalPosition(), transform->GetLocalRotation(), scale);

	D2D1::Matrix3x2F worldMatrix = myMatrix * parentMatrix * GetCameraInverseTransformMatrix(camera);
	deviceContext->SetTransform(worldMatrix);
}

void MatrixTransform::SetUITransformMatrixWithAdditionalScaling(ID2D1DeviceContext* deviceContext, const Transform* transform, const Vector2 additionalScaling, const Camera* camera)
{
	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return;
		}
	}

	D2D1::Matrix3x2F parentMatrix = D2D1::Matrix3x2F::Identity();
	if (transform->GetParent() != nullptr)
	{
		parentMatrix = parentMatrix * GetWorldTransformMatrix(transform->GetParent());
	}

	Vector3 scale = transform->GetLocalScale();
	scale.x = scale.x * additionalScaling.x;
	scale.y = scale.y * additionalScaling.y;
	D2D1::Matrix3x2F myMatrix = GetWorldTransformMatrix(transform->GetLocalPosition(), transform->GetLocalRotation(), scale);

	D2D1::Matrix3x2F cameraInverse = GetCameraToViewTransformMatrix(camera);
	D2D1::Matrix3x2F UIMatrix = myMatrix * parentMatrix * cameraInverse;
	deviceContext->SetTransform(UIMatrix);
}