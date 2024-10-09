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

	// 자식 * 부모 순으로 곱해야 함. 로컬 좌표계에 월드 행렬을 곱하듯이, 완성된 자식 행렬을 부모 행렬 기준으로의 상대적 변환을 가하는 것이기 때문이다.
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

	// 카메라가 부착된 오브젝트가 자식 오브젝트인 경우, 우선 부모의 월드 행렬을 구한 뒤 곱한다.
	D2D1::Matrix3x2F worldMatrix = D2D1::Matrix3x2F::Identity();
	if (cameraTransform->GetParent() != nullptr)
	{
		worldMatrix = GetWorldTransformMatrix(cameraTransform->GetParent());
	}

	// 카메라의 경우, 변환 기준은 중점인 카메라가 부착된 오브젝트의 포지션이 아니라, 카메라의 좌측 상단 꼭지점(뷰포트 좌표 (0, 0))이 되어야 한다.
	// 따라서 Translation은 카메라의 좌상단 꼭지점으로 이동하지만, Scale, Rotation은 카메라의 중점을 기준으로 수행되어야 한다.
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