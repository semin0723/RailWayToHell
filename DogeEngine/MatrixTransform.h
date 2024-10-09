#pragma once
#include "DirectXFramework.h"
#include "Vector3.h"

class Transform;
class Camera;

class MatrixTransform
{
public:
	// <24-07-02> GetTransform �� �Լ��� �θ��� ��ġ�� ��� �ݿ��� ���� ��ȯ�ϰ�, �̸� ���� �� ���� TRS ��İ��� �ϴ� ����� 2D���� ������ �� ���� �θ� �ڽ� ���谡 �ſ� ���������� 3D���� ����ϱ� ����� ������ ����. �׷��� �θ��� ��Ʈ������ �ڽ��� ��Ʈ������ ���ϴ� ������� ����.

	// ���޵� position, rotation, scale�� �������� ��ȯ�� ���� ����� ��ȯ�մϴ�. �θ� ������Ʈ�� ���� �� ���� ����� ���ϱ� ���� ����մϴ�.
	static D2D1::Matrix3x2F GetWorldTransformMatrix(const Vector3& position, const Vector3& rotation, const Vector3& scale);
	// ���޵� Transform�� �������� ��ȯ�� ���� ����� ��ȯ�մϴ�. ��ȯ���� �θ� Transform�� ������ ��� ��������� ���� ����Դϴ�.
	static D2D1::Matrix3x2F GetWorldTransformMatrix(const Transform* transform);
	// ���޵� ī�޶��� �𵨸� ����� ������� ��ȯ�մϴ�.
	static D2D1::Matrix3x2F GetCameraInverseTransformMatrix(const Camera* camera);
	// ���޵� ī�޶��� �ʺ�� ���̸� ����Ͽ� �»���� (0, 0)�� ����Ʈ ��ǥ�� �̵���ȯ�ϱ� ���� ī�޶� ������� ��ȯ�մϴ�. UI �׸��⿡ ����մϴ�.
	static D2D1::Matrix3x2F GetCameraToViewTransformMatrix(const Camera* camera);

	// DeviceContext�� ������ ����� �׵���ķ� �ʱ�ȭ�մϴ�.
	static void ClearMatrix(ID2D1DeviceContext* deviceContext);
	// ���޵� DeviceContext�� ������ ����� ������Ʈ�� ���� ��ǥ ��ķ� ��ȯ�մϴ�.
	static void SetWorldTransformMatrix(ID2D1DeviceContext* deviceContext, const Transform* transform, const Camera* camera = nullptr);
	// ���޵� DeviceContext�� ������ ����� ������Ʈ�� ���� ��ǥ ��ķ� ��ȯ�մϴ�. ���� Transform ���� ������ ����� ������ �� ����մϴ�.
	static void SetWorldTransformMatrix(ID2D1DeviceContext* deviceContext, const Vector3& position, const Vector3& rotation, const Vector3& scale, const Camera* camera = nullptr);
	// ���޵� DeviceContext�� ������ ����� UI�� ��ũ�� ��ǥ ��ķ� ��ȯ�մϴ�. UI�� ��ũ�� ��ǥ�� ����ϸ� ī�޶� ������� �ʽ��ϴ�.
	static void SetUITransformMatrix(ID2D1DeviceContext* deviceContext, const Transform* transform, const Camera* camera = nullptr);
	// ���޵� DeviceContext�� ������ ����� UI�� ��ũ�� ��ǥ ��ķ� ��ȯ�մϴ�. UI�� ��ũ�� ��ǥ�� ����ϸ� ī�޶� ������� �ʽ��ϴ�. ���� Transform ���� ������ ����� ������ �� ����մϴ�.
	static void SetUITransformMatrix(ID2D1DeviceContext* deviceContext, const Vector3& position, const Vector3& rotation, const Vector3& scale, const Camera* camera = nullptr);

	// ���޵� DeviceContext�� ������ ����� ������Ʈ�� ���� ��ǥ ��ķ� ��ȯ�մϴ�. D2DDrawImage�� ��� D2DDrawBitmap�� �޸� �׸� ������ RECT�� �ƴ� �»�� ����Ʈ�θ� ������ �� �ֱ� ������, �׸� ������ �����ϱ� ���� �����ϸ��� �ʿ��� �� ����մϴ�.
	static void SetWorldTransformMatrixWithAdditionalScaling(ID2D1DeviceContext* deviceContext, const Transform* transform, const Vector2 additionalScaling, const Camera* camera = nullptr);
	// ���޵� DeviceContext�� ������ ����� UI�� ��ũ�� ��ǥ ��ķ� ��ȯ�մϴ�. UI�� ��ũ�� ��ǥ�� ����ϸ� ī�޶� ������� �ʽ��ϴ�. D2DDrawImage�� ��� D2DDrawBitmap�� �޸� �׸� ������ RECT�� �ƴ� �»�� ����Ʈ�θ� ������ �� �ֱ� ������, �׸� ������ �����ϱ� ���� �����ϸ��� �ʿ��� �� ����մϴ�.
	static void SetUITransformMatrixWithAdditionalScaling(ID2D1DeviceContext* deviceContext, const Transform* transform, const Vector2 additionalScaling, const Camera* camera = nullptr);




	// ������-------------------------------

	// �׵���Ŀ� ���� ���
	//static void SRTMatrix(ID2D1DeviceContext* deviceContext, Vector3 Position, Vector3 Rotation, Vector3 Scale, DogeEngine::Component::Camera* camera = nullptr);
	//static void SRTMatrix(ID2D1DeviceContext* deviceContext, DogeEngine::Component::Transform* transform, DogeEngine::Component::Camera* camera = nullptr);

	//static void SRTMatrixForUI(ID2D1DeviceContext* deviceContext, Vector3 Position, Vector3 Rotation, Vector3 Scale, DogeEngine::Component::Camera* camera = nullptr);
	//static void SRTMatrixForUI(ID2D1DeviceContext* deviceContext, DogeEngine::Component::Transform* transform, DogeEngine::Component::Camera* camera = nullptr);


	//// Effected ��ü�� �׸� �� ����ϴ� DrawImage �Լ����� �׸� ������ ũ�⸦ ������ �� ����. ���� �׸�ŭ ��ǥ�踦 �����ϸ��ؼ� �׸�
	//// �ʱ� ��Ŀ� ���� ���
	//static void SRTMatrixWithAdditionalScaling(ID2D1DeviceContext* deviceContext, Vector3 Position, Vector3 Rotation, Vector3 Scale, Vector2 additionalScaling, DogeEngine::Component::Camera* camera = nullptr);
	//static void SRTMatrixForUIWithAdditionalScaling(ID2D1DeviceContext* deviceContext, Vector3 Position, Vector3 Rotation, Vector3 Scale, Vector2 additionalScaling, DogeEngine::Component::Camera* camera = nullptr);
};