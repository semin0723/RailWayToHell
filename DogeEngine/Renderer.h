#pragma once
#include "DirectXFramework.h"
#include "Vector2.h"
#include "Structures.h"
#include "defines.h"
#include "TextRenderer.h"

class Transform;
class SpriteRenderer;
class Camera;

NAMESPACE_DOGEENGINE_BEGIN

class Renderer
{
public:
	bool enabled = true;
	bool useAABBCulling = true;
	bool useOBBCulling = false;

	// FlipDC �Ǵ� EndDraw
	virtual void BeginDraw() abstract;
	virtual void UpdateRenderer() abstract;

	virtual void UpdateBitmapSize(int width, int height) abstract;

	virtual ~Renderer() {}
};


class D2DRenderer : public Renderer
{
	static ComPtr<ID2D1Factory1> d2dFactory;
	static ComPtr<IDXGIDevice> dxgiDevice;

	ComPtr<ID2D1Device> d2dDevice = nullptr;
	ComPtr<ID2D1DeviceContext> deviceContext = nullptr;
	ComPtr<ID3D11Device> d3dDevice = nullptr;
	ComPtr<IDXGISwapChain> dxgiSwapChain = nullptr;
	ComPtr<IDXGISurface> dxgiSurface = nullptr;
	ComPtr<ID2D1Bitmap1> outputBitmap = nullptr;

	// VSync�� ����� ��� DirectX �������� �����ӷ���Ʈ�� ������(���ӷ����� ��ε�) / ������� ���� ��� ������ swapInterval�� ����Ͽ� �׸��� Ƚ���� ����(���� ������ ��ε��� ����)
	bool useVSync = true;
	const float swapChainUpdateInterval = 0.015f;
	float elapsedTime = swapChainUpdateInterval;
	bool draw = false;
	bool replace_bitmap_on_screen_size_change = true;
public:
	Camera* camera = nullptr;
	D2DRenderer(int width, int height, Camera* camera);

	ComPtr<IDXGIDevice> GetDxgiDevice();
	ComPtr<ID2D1DeviceContext> GetDeviceContext();
	ComPtr<ID2D1Bitmap1> GetOutputBitmap();
	void ChangeCamera(Camera* camera);
	Camera* GetCamera();
	ComPtr<IDXGISwapChain> GetDXGISwapChain();

	void BeginDraw() override;
	void UpdateRenderer() override;
	void UpdateBitmapSize(int width, int height) override;

	void SetReplaceBitmapOnScreenSizeChanged(bool set);
	bool GetReplaceBitmapOnScreenSizeChanged() const;



	// ------- Image -------
private:
	bool Culling(SpriteRenderer* renderer);
public:
	// ��Ʈ�� �׸���: ����Ʈ ���� �Ұ� / DrawImage���� ����
	void DrawBitmap(ID2D1Bitmap* image, RectF rect, RectF sourceRect, float opacity, Transform* transform, SpriteRenderer* renderer, bool drawingUI = false);
	// �̹��� �׸���: ����Ʈ ���� ���� / DrawBitmap���� ����
	void DrawEffectedImage(ID2D1Effect* effectedImage, RectF rect, RectF sourceRect, Transform* transform, SpriteRenderer* renderer, bool drawingUI = false);

	// ------- Primitive Shapes -------
	void DrawLine(Vector2 start, Vector2 end, Color color, float width = 1.0f, bool drawingUI = false);
	void DrawSquare(Vector2 min, Vector2 max, Color lineColor, float lineWidth = 1.0f, bool drawingUI = false);
	void FillSquare(Vector2 min, Vector2 max, Color color, bool drawingUI = false);
	void DrawCircle(Vector2 center, float radius, Color lineColor, float lineWidth = 1.0f, bool drawingUI = false);
	void FillCircle(Vector2 center, float radius, Color color, bool drawingUI = false);

	// ------- Text -------
	void PrintText(Vector3 leftTopAnchor, Vector3 rotation, Vector3 scale, const wchar_t* text, Color color, const wchar_t* fontName, ComPtr<IDWriteFontCollection1> fontCollection, float fontSize, TextAlignVertical alignVertical, TextAlignHorizontal alignHorizontal, TextWeight weight, TextStyle style, bool drawingUI);
};

NAMESPACE_DOGEENGINE_END