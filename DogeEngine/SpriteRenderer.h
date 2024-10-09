#pragma once
#include <string>
#include "DirectXFramework.h"
#include "Component.h"
#include "Structures.h"
#include "Vector2.h"
#include "NineSlicedImage.h"

// ���߿� ����, ���� Anchor �����ؼ� ���� ��
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
	
	// Ư�� ��Ʈ�ʰ� �����Ű�� ���� ����Ʈ ��ü�� �޾Ƽ� �׸� �� image�� ������Ѽ� �׸�
	ComPtr<ID2D1Effect> additionalEffect = nullptr; // Color Matrix ���� �߰� ����Ʈ
	Vector2 rawSize{ 100, 100 }; // Ŭ���̾�Ʈ�� �׷����� ������

	void DrawBitmap(RectF rect, RectF sourceRect);
public:
	// �ӽ�
	bool useNineSliced = false;
	NineSlicedImage* nineSlicedImage = nullptr;

	// Full, Crop�� ���� AnchorType�� ������ �� ����.
	AnchorType anchorType{ AnchorType::Center };
	Vector2 anchor{ 0,0 };
	RectF sourceRect; // �̹����� �׷����� �κ��� ����
	Color color{ 1.f, 1.f, 1.f, 1.f };
	SpriteMode spriteMode = SpriteMode::Full;
	bool applyAdditionalEffect = false; // Color Matrix�� �⺻ ����, �� �� ����Ʈ�� �ɼ� �Ѿ� ��
	bool useAABBCulling = true; // D2D�� �˾Ƽ� �ø� ���شٴ��� ���󿴴�
	bool useOBBCulling = false;

	// SpriteMode::Repeat �� ��
	float pixelSewingX = 0.0f; // ��Ȯ�� ��ġ�� �׸��� ������ ������ �� 1px�� ���̰� �������� ������ ����
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

	// �׷����� ������ �ٿ�� �ڽ� (ī�޶�� ������Ʈ�� ȸ�� �� �������� �����)�� �����¿� ���� �������� ��ȯ�մϴ�.
	// <24-07-02> �̿ϼ�. ī�޶��� �����̼��� ����� ó������ �� ��.
	RectF GetBoundingBoxRect();
	// <24-07-02> �̿ϼ�. ī�޶��� �����̼��� ����� ó������ �� ��.
	void DrawBoundingBox();
};