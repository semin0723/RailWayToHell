#pragma once
#include "DirectXFramework.h"
#include "StandardFramework.h"
#include "CustomFont.h"

class Transform;
class SpriteRenderer;
class TextRenderer;

class DrawingManager
{
	DrawingManager() {}
	~DrawingManager() {}
public:
	// ------- Image -------
	// ��Ʈ�� �׸���: ����Ʈ ���� �Ұ� / DrawImage���� ����
	static void DrawBitmap(ID2D1Bitmap* image, RectF rect, RectF sourceRect, float opacity, Transform* transform, SpriteRenderer* renderer, bool drawingUI = false);
	static void DrawBitmap(ID2D1Bitmap* image, RectF rect, RectF sourceRect, Transform* transform, SpriteRenderer* renderer, bool drawingUI = false);

	// �̹��� �׸���: ����Ʈ ���� ���� / DrawBitmap���� ����
	static void DrawEffectedImage(ID2D1Effect* effectedImage, RectF rect, RectF sourceRect, Transform* transform, SpriteRenderer* renderer, bool drawingUI = false);




	// ------- Primitive Shapes -------
	static void DrawLine(Vector2 start, Vector2 end, Color color, float width = 1.0f, bool drawingUI = false);
	static void DrawLine(Vector2 anchor, Vector2 direction, float length, Color color, float width = 1.0f, bool drawingUI = false);
	static void DrawLine(Vector3 anchor, Vector3 direction, float length, Color color, float width = 1.0f, bool drawingUI = false);

	static void DrawSquare(Vector2 min, Vector2 max, Color lineColor, float lineWidth = 1.0f, bool drawingUI = false);
	static void DrawSquare(Vector2 anchor, float width, float height, Color lineColor, float lineWidth = 1.0f, bool drawingUI = false);
	static void DrawSquare(std::vector<Vector2> points, Color lineColor, float lineWidth = 1.0f, bool drawingUI = false);
	static void FillSquare(Vector2 min, Vector2 max, Color color, bool drawingUI = false);
	static void FillSquare(Vector2 anchor, float width, float height, Color color, bool drawingUI = false);

	static void DrawCircle(Vector2 center, float radius, Color lineColor, float lineWidth = 1.0f, bool drawingUI = false);
	static void FillCircle(Vector2 center, float radius, Color color, bool drawingUI = false);


	// ------- Text -------
	// �⺻ ������ ��Ʈ �ݷ��� ���
	static void PrintText(const wchar_t* text, Vector2 position, Color color, const wchar_t* fontName = L"Arial", float fontSize = 20.f, bool drawingUI = false);
	// ������ �߰��� ��Ʈ �ݷ��� ��� / ������ �߰��� ��Ʈ�� ��Ʈ���� ������ ��Ʈ �ݷ����� �����ϹǷ� ���� �̸��� ���� �ʴ´�.
	static void PrintText(const wchar_t* text, Vector2 position, Color color, CustomFont* font, float fontSize = 20.f, bool drawingUI = false);
	static void PrintText(TextRenderer* textRenderer);
};
