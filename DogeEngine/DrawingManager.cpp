#include <unordered_map>
#include "DrawingManager.h"
#include "Core.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "Scene.h"
#include "Object.h"

// SRTMatrix�� ��ġ�� ī�޶��� ���� ����� Ŭ���̾�Ʈ ������ (0, 0)�� �� ���� ������Ʈ�� ��ġ, ȸ��, ũ�� ������ ���ĵȴ�.
		// �࿡ ���� ���� ��ȯ�̹Ƿ� ��ȯ �� ���޵� ���� ��ġ, ȸ��, ũ�Ⱚ�� �ش� ��ǥ���� (0, 0) ������ �ǹǷ� ���� �׸��� �۾��� (0, 0)�� �������� ����Ǿ�� �Ѵ�. ������ start ���ͺ��� end ���ͱ��� ���� �ߴ´ٸ�, start�� �������� SRTMatrix ��ȯ�� ������ ��� start ������ (0, 0)���� ���ĵǹǷ� (0, 0)�������� end - start ���� �׸��� �ȴ�.


void DrawingManager::DrawBitmap(ID2D1Bitmap* image, RectF rect, RectF sourceRect, float opacity, Transform* transform, SpriteRenderer* renderer, bool drawingUI)
{
	DG::Core::GetInstance().GetMainRenderer()->DrawBitmap(image, rect, sourceRect, opacity, transform, renderer, drawingUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->DrawBitmap(image, rect, sourceRect, opacity, transform, renderer, drawingUI);
	}
}
void DrawingManager::DrawBitmap(ID2D1Bitmap* image, RectF rect, RectF sourceRect, Transform* transform, SpriteRenderer* renderer, bool drawingUI)
{
	DrawBitmap(image, rect, sourceRect, 1.0f, transform, renderer, drawingUI);
}

void DrawingManager::DrawEffectedImage(ID2D1Effect* effectedImage, RectF rect, RectF sourceRect, Transform* transform, SpriteRenderer* renderer, bool drawingUI)
{
	DG::Core::GetInstance().GetMainRenderer()->DrawEffectedImage(effectedImage, rect, sourceRect, transform, renderer, drawingUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->DrawEffectedImage(effectedImage, rect, sourceRect, transform, renderer, drawingUI);
	}
}





// SRTMatrix �Լ� �ȿ����� ��������, ī�޶��� �������� Ŀ���ٴ� �� ī�޶� ��� ������ �þ�ٴ� �Ҹ�����, ���� ��ǥ�踦 �������� �� ���� ������Ʈ�� ũ�Ⱑ �۾�����(��, ���� ��ǥ�谡 ��ҵǾ��) �׷����� ������ �о��� ���̹Ƿ�, ī�޶��� �������� ������ ���ؼ� ����ؾ� �Ѵ�.
// �׷��� ���� ��ǥ�� ���������� ���� �����Ϸ� ������ ���ڸ��� ������� �ϴµ�, ���⼭�� ���� �������̶� ������ �����Ƿ� ������ ���ߴ� cameraScale�� �ٽ� ������ ���� �������ָ� �ȴ�. (�Ǵ�, ������ ���� cameraScale�� ������ �� �ƴ϶� ��������)
void DrawingManager::DrawLine(Vector2 start, Vector2 end, Color color, float width, bool drawingUI)
{
	DG::Core::GetInstance().GetMainRenderer()->DrawLine(start, end, color, width, drawingUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->DrawLine(start, end, color, width, drawingUI);
	}
}
void DrawingManager::DrawLine(Vector2 anchor, Vector2 direction, float length, Color color, float width, bool drawingUI)
{
	Vector2 start = anchor;
	Vector2 end = anchor + (direction.Normalize() * length);
	DrawLine(start, end, color, width);
}
void DrawingManager::DrawLine(Vector3 anchor, Vector3 direction, float length, Color color, float width, bool drawingUI)
{
	DrawLine(anchor.ToVector2(), direction.ToVector2(), length, color, width);
}



void DrawingManager::DrawSquare(Vector2 min, Vector2 max, Color lineColor, float lineWidth, bool drawingUI)
{
	DG::Core::GetInstance().GetMainRenderer()->DrawSquare(min, max, lineColor, lineWidth, drawingUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->DrawSquare(min, max, lineColor, lineWidth, drawingUI);
	}
}
void DrawingManager::DrawSquare(Vector2 anchor, float width, float height, Color lineColor, float lineWidth, bool drawingUI)
{
	Vector2 leftBottom(anchor.x - width / 2, anchor.y - height / 2);
	Vector2 rightTop(anchor.x + width / 2, anchor.y + height / 2);

	DrawSquare(leftBottom, rightTop, lineColor, lineWidth);
}

void DrawingManager::DrawSquare(std::vector<Vector2> points, Color lineColor, float lineWidth, bool drawingUI)
{
	for (int i = 0; i < points.size() - 1; ++i)
	{
		DrawingManager::DrawLine(points[i], points[i + 1], lineColor, lineWidth, drawingUI);
	}
	DrawingManager::DrawLine(points[points.size() - 1], points[0], lineColor, lineWidth, drawingUI);
}



void DrawingManager::FillSquare(Vector2 min, Vector2 max, Color color, bool drawingUI)
{
	DG::Core::GetInstance().GetMainRenderer()->FillSquare(min, max, color, drawingUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->FillSquare(min, max, color, drawingUI);
	}
}
void DrawingManager::FillSquare(Vector2 anchor, float width, float height, Color color, bool drawingUI)
{
	Vector2 leftBottom(anchor.x - width / 2, anchor.y - height / 2);
	Vector2 rightTop(anchor.x + width / 2, anchor.y + height / 2);

	FillSquare(leftBottom, rightTop, color);
}



void DrawingManager::DrawCircle(Vector2 center, float radius, Color lineColor, float lineWidth, bool drawingUI)
{
	DG::Core::GetInstance().GetMainRenderer()->DrawCircle(center, radius, lineColor, lineWidth, drawingUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->DrawCircle(center, radius, lineColor, lineWidth, drawingUI);
	}
}

void DrawingManager::FillCircle(Vector2 center, float radius, Color color, bool drawingUI)
{
	DG::Core::GetInstance().GetMainRenderer()->FillCircle(center, radius, color, drawingUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->FillCircle(center, radius, color, drawingUI);
	}
}




void PrintText(Vector3 leftTopAnchor, Vector3 rotation, Vector3 scale, const wchar_t* text, Color color, const wchar_t* fontName, ComPtr<IDWriteFontCollection1> fontCollection, float fontSize, TextAlignVertical alignVertical, TextAlignHorizontal alignHorizontal, TextWeight weight, TextStyle style, bool drawingUI);

void DrawingManager::PrintText(const wchar_t* text, Vector2 position, Color color, const wchar_t* fontName, float fontSize, bool drawingUI)
{
	DG::Core::GetInstance().GetMainRenderer()->PrintText(position, { 0, 0, 0 }, { 1, 1, 1 }, text, color, fontName, nullptr, fontSize, TextAlignVertical::Top, TextAlignHorizontal::Left, TextWeight::Normal, TextStyle::Normal, drawingUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->PrintText(position, { 0, 0, 0 }, { 1, 1, 1 }, text, color, fontName, nullptr, fontSize, TextAlignVertical::Top, TextAlignHorizontal::Left, TextWeight::Normal, TextStyle::Normal, drawingUI);
	}
}

void DrawingManager::PrintText(const wchar_t* text, Vector2 position, Color color, CustomFont* font, float fontSize, bool drawingUI)
{
	DG::Core::GetInstance().GetMainRenderer()->PrintText(position, { 0, 0, 0 }, { 1, 1, 1 }, text, color, font->fontName.c_str(), font->fontCollection, fontSize, TextAlignVertical::Top, TextAlignHorizontal::Left, TextWeight::Normal, TextStyle::Normal, drawingUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->PrintText(position, { 0, 0, 0 }, { 1, 1, 1 }, text, color, font->fontName.c_str(), font->fontCollection, fontSize, TextAlignVertical::Top, TextAlignHorizontal::Left, TextWeight::Normal, TextStyle::Normal, drawingUI);
	}
}

void DrawingManager::PrintText(TextRenderer* tr)
{
	Object* obj = tr->GetOwner();
	Vector3 position = obj->transform->GetPosition();
	Vector3 rotation = obj->transform->GetRotation();
	Vector3 scale = obj->transform->GetScale();

	bool isUI = obj->GetObjectLayer() == ObjectLayer::UI ? true : false;


	DG::Core::GetInstance().GetMainRenderer()->PrintText(position, rotation, scale, tr->text.c_str(), tr->color, tr->font->fontName.c_str(), tr->font->fontCollection, tr->fontSize, tr->alignVertical, tr->alignHorizontal, tr->weight, tr->style, isUI);

	std::unordered_map<std::string, std::shared_ptr<DG::D2DRenderer>>& additionalRenderer = DG::Core::GetInstance().GetCurrentScene()->additionalRenderer;
	for (auto p : additionalRenderer)
	{
		if (p.second.get()->enabled == false) continue;

		p.second.get()->PrintText(position, rotation, scale, tr->text.c_str(), tr->color, tr->font->fontName.c_str(), tr->font->fontCollection, tr->fontSize, tr->alignVertical, tr->alignHorizontal, tr->weight, tr->style, isUI);
	}
}
