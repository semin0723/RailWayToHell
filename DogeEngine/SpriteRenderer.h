#pragma once
#include <string>
#include "DirectXFramework.h"
#include "Component.h"
#include "Structures.h"
#include "Vector2.h"
#include "NineSlicedImage.h"

// 나중에 세로, 가로 Anchor 구분해서 나눌 것
enum class AnchorType
{
	Center, BottomCenter, TopCenter, LeftCenter, RightCenter
};

enum class SpriteMode
{
	Full, Crop, Repeat, FullRepeat
};

class SpriteRenderer : public Component
{
	ComPtr<ID2D1Bitmap> image = nullptr;
	
	// 특정 비트맵과 연결시키지 않은 이펙트 객체만 받아서 그릴 때 image랑 연결시켜서 그림
	ComPtr<ID2D1Effect> additionalEffect = nullptr; // Color Matrix 제외 추가 이펙트
	Vector2 rawSize{ 100, 100 }; // 클라이언트에 그려지는 사이즈

	void DrawBitmap(RectF rect, RectF sourceRect);
public:
	// 임시
	bool useNineSliced = false;
	NineSlicedImage* nineSlicedImage = nullptr;

	// Full, Crop일 때만 AnchorType을 구분할 수 있음.
	AnchorType anchorType{ AnchorType::Center };
	Vector2 anchor{ 0,0 };
	RectF sourceRect; // 이미지의 그려지는 부분의 영역
	Color color{ 1.f, 1.f, 1.f, 1.f };
	SpriteMode spriteMode = SpriteMode::Full;
	bool applyAdditionalEffect = false; // Color Matrix는 기본 적용, 그 외 이펙트는 옵션 켜야 함
	bool useAABBCulling = true; // D2D가 알아서 컬링 해준다더니 구라였다
	bool useOBBCulling = false;

	// SpriteMode::Repeat 일 때
	float pixelSewingX = 0.0f; // 정확한 위치에 그리면 빠르게 움직일 때 1px씩 사이가 벌어지는 현상이 생김
	float pixelSewingY = 0.0f;
	bool ignoreResidue = false;

	~SpriteRenderer();
	void Draw();

	void SetAdditionalEffect(ComPtr<ID2D1Effect> additionalEffect);
	void DeleteAdditionalEffect();

	void SetImage(ComPtr<ID2D1Bitmap> image, RectF sourceRect, bool autoSizing = false);
	bool SetImage(const char* key, bool autoSizing = false);
	bool SetImage(std::string key, bool autoSizing = false);
	void SetImage(ComPtr<ID2D1Bitmap> image, bool autoSizing = false);
	ComPtr<ID2D1Bitmap> GetImage();

	void SetSize(float _sizeX, float _sizeY);
	void SetSizeAuto();
	void MultSize(float mult);
	void SetWidth(float _width);
	void SetHeight(float _height);
	void SetSizeByWidth(float _width);
	void SetSizeByHeight(float _height);

	float GetWidth();
	float GetHeight();

	// 그려지는 영역의 바운딩 박스 (카메라와 오브젝트의 회전 및 스케일을 고려함)의 상하좌우 월드 포지션을 반환합니다.
	// <24-07-02> 미완성. 카메라의 로테이션을 제대로 처리하지 못 함.
	RectF GetBoundingBoxRect();
	// <24-07-02> 미완성. 카메라의 로테이션을 제대로 처리하지 못 함.
	void DrawBoundingBox();
};