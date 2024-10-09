#pragma once
#include "DirectXFramework.h"
#include "Vector3.h"

class Transform;
class Camera;

class MatrixTransform
{
public:
	// <24-07-02> GetTransform 류 함수로 부모의 위치를 모두 반영한 값을 반환하고, 이를 통해 한 번의 TRS 행렬곱만 하는 방식은 2D에선 괜찮을 지 몰라도 부모 자식 관계가 매우 복잡해지는 3D에선 사용하기 어려울 것으로 보임. 그래서 부모의 매트릭스에 자식의 매트릭스를 곱하는 방식으로 변경.

	// 전달된 position, rotation, scale을 기준으로 변환된 월드 행렬을 반환합니다. 부모 오브젝트가 없을 때 월드 행렬을 구하기 위해 사용합니다.
	static D2D1::Matrix3x2F GetWorldTransformMatrix(const Vector3& position, const Vector3& rotation, const Vector3& scale);
	// 전달된 Transform을 기준으로 변환된 월드 행렬을 반환합니다. 반환값은 부모 Transform이 존재할 경우 재귀적으로 곱한 결과입니다.
	static D2D1::Matrix3x2F GetWorldTransformMatrix(const Transform* transform);
	// 전달된 카메라의 모델링 행렬의 역행렬을 반환합니다.
	static D2D1::Matrix3x2F GetCameraInverseTransformMatrix(const Camera* camera);
	// 전달된 카메라의 너비와 높이를 고려하여 좌상단이 (0, 0)인 뷰포트 좌표로 이동변환하기 위한 카메라 역행렬을 반환합니다. UI 그리기에 사용합니다.
	static D2D1::Matrix3x2F GetCameraToViewTransformMatrix(const Camera* camera);

	// DeviceContext의 설정된 행렬을 항등행렬로 초기화합니다.
	static void ClearMatrix(ID2D1DeviceContext* deviceContext);
	// 전달된 DeviceContext에 설정된 행렬을 오브젝트의 월드 좌표 행렬로 변환합니다.
	static void SetWorldTransformMatrix(ID2D1DeviceContext* deviceContext, const Transform* transform, const Camera* camera = nullptr);
	// 전달된 DeviceContext에 설정된 행렬을 오브젝트의 월드 좌표 행렬로 변환합니다. 기준 Transform 없이 임의의 행렬을 적용할 때 사용합니다.
	static void SetWorldTransformMatrix(ID2D1DeviceContext* deviceContext, const Vector3& position, const Vector3& rotation, const Vector3& scale, const Camera* camera = nullptr);
	// 전달된 DeviceContext에 설정된 행렬을 UI의 스크린 좌표 행렬로 변환합니다. UI는 스크린 좌표를 사용하며 카메라를 고려하지 않습니다.
	static void SetUITransformMatrix(ID2D1DeviceContext* deviceContext, const Transform* transform, const Camera* camera = nullptr);
	// 전달된 DeviceContext에 설정된 행렬을 UI의 스크린 좌표 행렬로 변환합니다. UI는 스크린 좌표를 사용하며 카메라를 고려하지 않습니다. 기준 Transform 없이 임의의 행렬을 적용할 때 사용합니다.
	static void SetUITransformMatrix(ID2D1DeviceContext* deviceContext, const Vector3& position, const Vector3& rotation, const Vector3& scale, const Camera* camera = nullptr);

	// 전달된 DeviceContext에 설정된 행렬을 오브젝트의 월드 좌표 행렬로 변환합니다. D2DDrawImage의 경우 D2DDrawBitmap과 달리 그릴 영역을 RECT가 아닌 좌상단 포인트로만 지정할 수 있기 때문에, 그릴 영역을 조절하기 위한 스케일링이 필요할 때 사용합니다.
	static void SetWorldTransformMatrixWithAdditionalScaling(ID2D1DeviceContext* deviceContext, const Transform* transform, const Vector2 additionalScaling, const Camera* camera = nullptr);
	// 전달된 DeviceContext에 설정된 행렬을 UI의 스크린 좌표 행렬로 변환합니다. UI는 스크린 좌표를 사용하며 카메라를 고려하지 않습니다. D2DDrawImage의 경우 D2DDrawBitmap과 달리 그릴 영역을 RECT가 아닌 좌상단 포인트로만 지정할 수 있기 때문에, 그릴 영역을 조절하기 위한 스케일링이 필요할 때 사용합니다.
	static void SetUITransformMatrixWithAdditionalScaling(ID2D1DeviceContext* deviceContext, const Transform* transform, const Vector2 additionalScaling, const Camera* camera = nullptr);




	// 공사중-------------------------------

	// 항등행렬에 대한 계산
	//static void SRTMatrix(ID2D1DeviceContext* deviceContext, Vector3 Position, Vector3 Rotation, Vector3 Scale, DogeEngine::Component::Camera* camera = nullptr);
	//static void SRTMatrix(ID2D1DeviceContext* deviceContext, DogeEngine::Component::Transform* transform, DogeEngine::Component::Camera* camera = nullptr);

	//static void SRTMatrixForUI(ID2D1DeviceContext* deviceContext, Vector3 Position, Vector3 Rotation, Vector3 Scale, DogeEngine::Component::Camera* camera = nullptr);
	//static void SRTMatrixForUI(ID2D1DeviceContext* deviceContext, DogeEngine::Component::Transform* transform, DogeEngine::Component::Camera* camera = nullptr);


	//// Effected 객체를 그릴 때 사용하는 DrawImage 함수에는 그릴 영역의 크기를 지정할 수 없다. 따라서 그만큼 좌표계를 스케일링해서 그림
	//// 초기 행렬에 대한 계산
	//static void SRTMatrixWithAdditionalScaling(ID2D1DeviceContext* deviceContext, Vector3 Position, Vector3 Rotation, Vector3 Scale, Vector2 additionalScaling, DogeEngine::Component::Camera* camera = nullptr);
	//static void SRTMatrixForUIWithAdditionalScaling(ID2D1DeviceContext* deviceContext, Vector3 Position, Vector3 Rotation, Vector3 Scale, Vector2 additionalScaling, DogeEngine::Component::Camera* camera = nullptr);
};