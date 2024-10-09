#include "SpriteRenderer.h"
#include "Transform.h"
#include "Object.h"
#include "Math.h"
#include "DrawingManager.h"
#include "EffectManager.h"
#include "ResourceManager.h"
#include "Camera.h"

#undef min
#undef max

void SpriteRenderer::DrawBitmap(RectF rect, RectF sourceRect)
{
	rect.left += anchor.x;
	rect.right += anchor.x;
	rect.top += anchor.y;
	rect.bottom += anchor.y;

	// 임시...
	if (useNineSliced)
	{
		Transform* transform = GetOwner()->transform;
		bool drawingUI = false;
		if (GetOwner()->GetObjectLayer() == ObjectLayer::Object)
		{
			drawingUI = false;
		}
		else if (GetOwner()->GetObjectLayer() == ObjectLayer::UI)
		{
			drawingUI = true;
		}
		DrawingManager::DrawBitmap(nineSlicedImage->image.Get(), rect, sourceRect, color.A, transform, this, drawingUI);
		return;
	}





	Transform* transform = GetOwner()->transform;
	bool drawingUI = false;
	if (GetOwner()->GetObjectLayer() == ObjectLayer::Object)
	{
		drawingUI = false;
	}
	else if (GetOwner()->GetObjectLayer() == ObjectLayer::UI)
	{
		drawingUI = true;
	}

	// Color Matrix 이펙트는 RGB값이 1이 아닐 때 기본 적용임
	if (Math::Approximate(color.R, 1.f) && Math::Approximate(color.G, 1.f) && Math::Approximate(color.B, 1.f))
	{
		DrawingManager::DrawBitmap(image.Get(), rect, sourceRect, color.A, transform, this, drawingUI);
	}
	else
	{
		ComPtr<ID2D1Effect> effected = DG::EffectManager::GetColorMatrixEffect(color);
		effected->SetInput(0, image.Get());
		DrawingManager::DrawEffectedImage(effected.Get(), rect, sourceRect, transform, this, drawingUI);
	}

	// 추가 이펙트 적용은 나중에...
}
SpriteRenderer::~SpriteRenderer()
{

}

void SpriteRenderer::Draw()
{
	// 임시...
	if (useNineSliced)
	{
		SizeF size = nineSlicedImage->image.Get()->GetSize();
		if (rawSize.x <= (float)nineSlicedImage->leftGap + (float)nineSlicedImage->rightGap + 0.1f) Assert(false, "rawSize is too small to draw 9-sliced image.");
		if (rawSize.y <= (float)nineSlicedImage->topGap + (float)nineSlicedImage->bottomGap + 0.1f) Assert(false, "rawSize is too small to draw 9-sliced image.");

		float lg = (float)nineSlicedImage->leftGap;
		float rg = (float)nineSlicedImage->rightGap;
		float tg = (float)nineSlicedImage->topGap;
		float bg = (float)nineSlicedImage->bottomGap;
		float halfWidth = rawSize.x * 0.5f;
		float halfHeight = rawSize.y * 0.5f;

		RectF rect;
		RectF sourceRect;

		// 좌상단 코너
		rect = { -halfWidth, -halfHeight, -halfWidth + lg, -halfHeight + tg};
		sourceRect = { 0.f, 0.f, lg, tg };
		DrawBitmap(rect, sourceRect);
		// 우상단 코너
		rect = { halfWidth - rg, -halfHeight, halfWidth, -halfHeight + tg };
		sourceRect = { size.width - rg, 0.f, size.width, tg};
		DrawBitmap(rect, sourceRect);
		// 좌하단 코너
		rect = { -halfWidth, halfHeight - bg, -halfWidth + lg, halfHeight };
		sourceRect = { 0.f, size.height - bg, lg, size.height};
		DrawBitmap(rect, sourceRect);
		// 우하단 코너
		rect = { halfWidth - rg, halfHeight - bg, halfWidth, halfHeight };
		sourceRect = { size.width - rg, size.height - bg, size.width, size.height};
		DrawBitmap(rect, sourceRect);

		// 상단
		rect = { -halfWidth + lg, -halfHeight, halfWidth - rg, -halfHeight + tg };
		sourceRect = { lg, 0.f, size.width - rg, tg };
		DrawBitmap(rect, sourceRect);
		// 하단
		rect = { -halfWidth + lg, halfHeight - bg, halfWidth - rg, halfHeight };
		sourceRect = { lg, size.height - bg, size.width - rg, size.height };
		DrawBitmap(rect, sourceRect);
		// 좌측
		rect = { -halfWidth, -halfHeight + tg, -halfWidth + lg, halfHeight - bg };
		sourceRect = { 0.f, tg, lg, size.height - bg };
		DrawBitmap(rect, sourceRect);
		// 우측
		rect = { halfWidth - rg, -halfHeight + tg, halfWidth, halfHeight - bg };
		sourceRect = { size.width - rg, tg, size.width, size.height - bg };
		DrawBitmap(rect, sourceRect);


		// 중앙
		rect = { -halfWidth + lg, -halfHeight + tg, halfWidth - rg, halfHeight - bg };
		sourceRect = { lg, tg, size.width - rg, size.height - bg };
		DrawBitmap(rect, sourceRect);

		return;
	}





	if (image == nullptr) return;

	if (spriteMode == SpriteMode::Full)
	{
		RectF rect;
		if (anchorType == AnchorType::Center) rect = { -rawSize.x / 2, -rawSize.y / 2, rawSize.x / 2, rawSize.y / 2 };
		else if (anchorType == AnchorType::BottomCenter) rect = { -rawSize.x / 2, -rawSize.y, rawSize.x / 2, 0 };
		else if (anchorType == AnchorType::TopCenter) rect = { -rawSize.x / 2, 0, rawSize.x / 2, rawSize.y };
		else if (anchorType == AnchorType::LeftCenter) rect = { 0, -rawSize.y / 2, rawSize.x, rawSize.y / 2 };
		else if (anchorType == AnchorType::RightCenter) rect = { -rawSize.x, -rawSize.y / 2, 0, rawSize.y / 2 };
		DrawBitmap(rect, sourceRect);
	}
	else if (spriteMode == SpriteMode::Crop)
	{
		RectF rect;
		if (anchorType == AnchorType::Center) rect = { -rawSize.x / 2, -rawSize.y / 2, rawSize.x / 2, rawSize.y / 2 };
		else if (anchorType == AnchorType::BottomCenter) rect = { -rawSize.x / 2, -rawSize.y, rawSize.x / 2, 0 };
		else if (anchorType == AnchorType::TopCenter) rect = { -rawSize.x / 2, 0, rawSize.x / 2, rawSize.y };

		float sourceWidth = sourceRect.right - sourceRect.left;
		float sourceHeight = sourceRect.bottom - sourceRect.top;

		float newWidth = sourceWidth < rawSize.x ? sourceWidth : rawSize.x;
		float newHeight = sourceHeight < rawSize.y ? sourceHeight : rawSize.y;

		RectF newSourceRect{ sourceRect.left, sourceRect.top, sourceRect.left + newWidth, sourceRect.top + newHeight };

		DrawBitmap(rect, newSourceRect);
	}
	else if (spriteMode == SpriteMode::Repeat)
	{
		Vector2 curPos{ -rawSize.x / 2, -rawSize.y / 2 };
		Vector2 endPos{ rawSize.x / 2, rawSize.y / 2 };

		float sourceWidth = sourceRect.right - sourceRect.left;
		float sourceHeight = sourceRect.bottom - sourceRect.top;

		if (sourceWidth < 0 || sourceHeight < 0) return;

		// 행에 대해 반복
		while (true)
		{
			float curSize_y = sourceHeight;
			if (curPos.y + curSize_y > endPos.y)
			{
				if (ignoreResidue == true) curSize_y = 0;
				else curSize_y = endPos.y - curPos.y;
				if (curSize_y <= 0.001f) break;
			}

			// 열 채우기
			while (true)
			{
				float curSize_x = sourceWidth;
				if (curPos.x + curSize_x > endPos.x)
				{
					if (ignoreResidue == true) curSize_x = 0;
					else curSize_x = endPos.x - curPos.x;
					if (curSize_x <= 0.001f) break;
				}

				RectF rect{ curPos.x, curPos.y, curPos.x + curSize_x, curPos.y + curSize_y };

				DrawBitmap(rect, sourceRect);

				curPos.x += sourceWidth - pixelSewingX;
			}
			curPos.x = -rawSize.x / 2; // 초기화
			curPos.y += sourceHeight - pixelSewingY;
		}
	}
	else if (spriteMode == SpriteMode::FullRepeat)
	{
		Vector2 curPos{ -rawSize.x / 2, -rawSize.y / 2 };
		Vector2 endPos{ rawSize.x / 2, rawSize.y / 2 };

		float imgWidth;
		float imgHeight;

		if (rawSize.x <= rawSize.y)
		{
			imgWidth = rawSize.x;
			imgHeight = rawSize.x;
		}
		else
		{
			imgWidth = rawSize.y;
			imgHeight = rawSize.y;
		}

		if (imgWidth < 0 || imgHeight < 0) return;

		while (true)
		{
			float curSize_y = imgHeight;
			if (curPos.y + curSize_y > endPos.y)
			{
				if (ignoreResidue == true) curSize_y = 0;
				else curSize_y = endPos.y - curPos.y;
				printf("1 curSize_y : %f\t endPos.y : %f\tcurPos.y : %f\n", curSize_y, endPos.y, curPos.y);
				if (curSize_y <= 0.001f) break;
			}
			else
			{
				printf("2 curSize_y : %f\t endPos.y : %f\tcurPos.y : %f\n", curSize_y, endPos.y, curPos.y);
			}
			// 열 채우기
			while (true)
			{
 				float curSize_x = imgWidth;
				if (curPos.x + curSize_x > endPos.x)
				{
					if (ignoreResidue == true) curSize_x = 0;
					else curSize_x = endPos.x - curPos.x;
					printf("1 curSize_x : %f\t endPos.x : %f\tcurPos.x : %f\n", curSize_x, endPos.x, curPos.x);
 					if (curSize_x <= 0.001f) break;
				}
				else
				{
					printf("2 curSize_x : %f\t endPos.x : %f\tcurPos.x : %f\n", curSize_x, endPos.x, curPos.x);
				}
				RectF rect{ curPos.x, curPos.y, curPos.x + curSize_x, curPos.y + curSize_y };

				DrawBitmap(rect, sourceRect);

				curPos.x += imgWidth - pixelSewingX;
			}
			curPos.x = -rawSize.x / 2; // 초기화
			curPos.y += imgHeight - pixelSewingY;
		}
	}
}


void SpriteRenderer::SetAdditionalEffect(ComPtr<ID2D1Effect> additionalEffect)
{
	this->additionalEffect = additionalEffect;
}
void SpriteRenderer::DeleteAdditionalEffect()
{
	if (additionalEffect == nullptr) return;

	additionalEffect.ReleaseAndGetAddressOf(); // Release하면 nullptr로 초기화까지 알아서 해줌
}


void SpriteRenderer::SetImage(ComPtr<ID2D1Bitmap> image, RectF sourceRect, bool autoSizing)
{
	this->image = image;
	this->sourceRect = sourceRect;

	if (autoSizing) SetSizeAuto();
}

bool SpriteRenderer::SetImage(const char* key, bool autoSizing)
{
	return SetImage(std::string(key), autoSizing);
}

bool SpriteRenderer::SetImage(std::string key, bool autoSizing)
{
	ComPtr<ID2D1Bitmap> bitmap = ResourceManager::GetImage(key);
	if (!bitmap)
	{

		return false;
	}
	SizeF size = bitmap.Get()->GetSize();
	RectF sourceRect{ 0, 0, size.width, size.height };
	SetImage(bitmap, sourceRect, autoSizing);
	return true;
}

void SpriteRenderer::SetImage(ComPtr<ID2D1Bitmap> _image, bool autoSizing)
{
	SizeF size = _image.Get()->GetSize();
	RectF sourceRect{ 0, 0, size.width, size.height };
	SetImage(_image, sourceRect, autoSizing);
}

ComPtr<ID2D1Bitmap> SpriteRenderer::GetImage()
{
	return image;
}

void SpriteRenderer::SetSize(float _sizeX, float _sizeY)
{
	rawSize.x = _sizeX;
	rawSize.y = _sizeY;
}
void SpriteRenderer::SetSizeAuto()
{
	if (image == nullptr) return;

	rawSize.x = image->GetSize().width;
	rawSize.y = image->GetSize().height;
}
void SpriteRenderer::MultSize(float mult)
{
	rawSize.x *= mult;
	rawSize.y *= mult;
}
void SpriteRenderer::SetWidth(float _width)
{
	rawSize.x = _width;
}
void SpriteRenderer::SetHeight(float _height)
{
	rawSize.y = _height;
}
void SpriteRenderer::SetSizeByWidth(float _width)
{
	float ratio = rawSize.x / rawSize.y;
	rawSize.x = _width;
	rawSize.y = _width / ratio;
}
void SpriteRenderer::SetSizeByHeight(float _height)
{
	float ratio = rawSize.x / rawSize.y;
	rawSize.x = _height * ratio;
	rawSize.y = _height;
}
float SpriteRenderer::GetWidth()
{
	return rawSize.x;
}
float SpriteRenderer::GetHeight()
{
	return rawSize.y;
}
RectF SpriteRenderer::GetBoundingBoxRect()
{
	// rawSize는 scale = 1일 때의 기본 사이즈를 의미하므로 그려지는 영역의 포지션을 전달할 때도 오브젝트의 scale 값을 고려해야 한다. (카메라의 스케일은 오브젝트의 월드좌표상 크기를 변경시키지 않음)
	Vector3 worldPos = GetOwner()->transform->GetPosition();
	Vector3 scale = GetOwner()->transform->GetScale();
	float rotation = GetOwner()->transform->GetRotation().z;
	rotation -= Camera::GetMainCamera()->rotation; // 카메라가 시계방향으로 돌면 오브젝트는 반시계방향으로 돈 것 처럼 그려진다.
	float halfWidth = rawSize.x * scale.x / 2.f;
	float halfHeight = rawSize.y * scale.y / 2.f;

	// 네 꼭지점의 로컬 좌표
	Vector3 lb{ -halfWidth, -halfHeight, 0 };
	Vector3 lt{ -halfWidth, halfHeight, 0 };
	Vector3 rt{ halfWidth, halfHeight, 0 };
	Vector3 rb{ halfWidth, -halfHeight, 0 };

	// 회전 한 경우 이를 고려해야 한다.
	if (!Math::Approximate(rotation, 0.0f))
	{
		float radian = Math::Deg2Rad(rotation);
		lb = lb.Rotate(radian);
		lt = lt.Rotate(radian);
		rt = rt.Rotate(radian);
		rb = rb.Rotate(radian);
	}

	// 바운딩 박스를 구하고 월드좌표로 변환해서 반환
	float left = std::min({ lb.x, lt.x, rt.x, rb.x }) + worldPos.x;
	float right = std::max({ lb.x, lt.x, rt.x, rb.x }) + worldPos.x;
	float top = std::max({ lb.y, lt.y, rt.y, rb.y }) + worldPos.y;
	float bottom = std::min({ lb.y, lt.y, rt.y, rb.y }) + worldPos.y;

	return RectF{ left, top, right, bottom };
}
void SpriteRenderer::DrawBoundingBox()
{
	float camRotation = Math::Deg2Rad(Camera::GetMainCamera()->rotation);
	RectF boundingBoxRect = GetBoundingBoxRect();
	Vector2 leftBottom{ boundingBoxRect.left, boundingBoxRect.bottom };
	Vector2 rightTop{ boundingBoxRect.right, boundingBoxRect.top };
	Vector2 leftTop{ boundingBoxRect.left, boundingBoxRect.top };
	Vector2 rightBottom{ boundingBoxRect.right, boundingBoxRect.bottom };

	/*float rotation = Math::Deg2Rad(GetOwner()->transform->GetRotation().z);
	leftBottom = leftBottom.Rotate(camRotation);
	rightTop = rightTop.Rotate(camRotation);
	leftTop = leftTop.Rotate(camRotation);
	rightBottom = rightBottom.Rotate(camRotation);*/

	//DrawingManager::DrawSquare(boundsMin, boundsMax, Color{ 0, 0, 1, 1 });
	DrawingManager::DrawLine(leftBottom, leftTop, Color{ 0,0,1,1 });
	DrawingManager::DrawLine(leftBottom, rightBottom, Color{ 0,0,1,1 });
	DrawingManager::DrawLine(leftTop, rightTop, Color{ 0,0,1,1 });
	DrawingManager::DrawLine(rightBottom, rightTop, Color{ 0,0,1,1 });
}