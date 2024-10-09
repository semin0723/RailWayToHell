#include <unordered_map>
#include "DrawingManager.h"
#include "Core.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "Scene.h"
#include "Object.h"

// SRTMatrix를 거치면 카메라의 좌측 상단이 클라이언트 영역의 (0, 0)이 될 때의 오브젝트의 위치, 회전, 크기 값으로 정렬된다.
		// 축에 대한 선형 변환이므로 변환 후 전달된 기준 위치, 회전, 크기값이 해당 좌표계의 (0, 0) 지점이 되므로 이후 그리는 작업은 (0, 0)을 기준으로 수행되어야 한다. 예컨대 start 벡터부터 end 벡터까지 선을 긋는다면, start를 기준으로 SRTMatrix 변환을 수행할 경우 start 지점이 (0, 0)으로 정렬되므로 (0, 0)에서부터 end - start 까지 그리면 된다.


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





// SRTMatrix 함수 안에서도 말했지만, 카메라의 스케일이 커진다는 건 카메라가 담는 영역이 늘어난다는 소리지만, 로컬 좌표계를 기준으로 볼 때는 오브젝트의 크기가 작아져야(즉, 로컬 좌표계가 축소되어야) 그려지는 영역이 넓어질 것이므로, 카메라의 스케일은 역원을 취해서 사용해야 한다.
// 그렇게 나온 좌표를 마지막으로 로컬 스케일로 나눠서 제자리에 맞춰줘야 하는데, 여기서는 로컬 스케일이란 개념이 없으므로 역원을 취했던 cameraScale에 다시 역원을 취해 나누어주면 된다. (또는, 역원을 취한 cameraScale을 나누는 게 아니라 곱해주자)
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
