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

	// �ӽ�...
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

	// Color Matrix ����Ʈ�� RGB���� 1�� �ƴ� �� �⺻ ������
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

	// �߰� ����Ʈ ������ ���߿�...
}
SpriteRenderer::~SpriteRenderer()
{

}

void SpriteRenderer::Draw()
{
	// �ӽ�...
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

		// �»�� �ڳ�
		rect = { -halfWidth, -halfHeight, -halfWidth + lg, -halfHeight + tg};
		sourceRect = { 0.f, 0.f, lg, tg };
		DrawBitmap(rect, sourceRect);
		// ���� �ڳ�
		rect = { halfWidth - rg, -halfHeight, halfWidth, -halfHeight + tg };
		sourceRect = { size.width - rg, 0.f, size.width, tg};
		DrawBitmap(rect, sourceRect);
		// ���ϴ� �ڳ�
		rect = { -halfWidth, halfHeight - bg, -halfWidth + lg, halfHeight };
		sourceRect = { 0.f, size.height - bg, lg, size.height};
		DrawBitmap(rect, sourceRect);
		// ���ϴ� �ڳ�
		rect = { halfWidth - rg, halfHeight - bg, halfWidth, halfHeight };
		sourceRect = { size.width - rg, size.height - bg, size.width, size.height};
		DrawBitmap(rect, sourceRect);

		// ���
		rect = { -halfWidth + lg, -halfHeight, halfWidth - rg, -halfHeight + tg };
		sourceRect = { lg, 0.f, size.width - rg, tg };
		DrawBitmap(rect, sourceRect);
		// �ϴ�
		rect = { -halfWidth + lg, halfHeight - bg, halfWidth - rg, halfHeight };
		sourceRect = { lg, size.height - bg, size.width - rg, size.height };
		DrawBitmap(rect, sourceRect);
		// ����
		rect = { -halfWidth, -halfHeight + tg, -halfWidth + lg, halfHeight - bg };
		sourceRect = { 0.f, tg, lg, size.height - bg };
		DrawBitmap(rect, sourceRect);
		// ����
		rect = { halfWidth - rg, -halfHeight + tg, halfWidth, halfHeight - bg };
		sourceRect = { size.width - rg, tg, size.width, size.height - bg };
		DrawBitmap(rect, sourceRect);


		// �߾�
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

		// �࿡ ���� �ݺ�
		while (true)
		{
			float curSize_y = sourceHeight;
			if (curPos.y + curSize_y > endPos.y)
			{
				if (ignoreResidue == true) curSize_y = 0;
				else curSize_y = endPos.y - curPos.y;
				if (curSize_y <= 0.001f) break;
			}

			// �� ä���
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
			curPos.x = -rawSize.x / 2; // �ʱ�ȭ
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
			// �� ä���
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
			curPos.x = -rawSize.x / 2; // �ʱ�ȭ
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

	additionalEffect.ReleaseAndGetAddressOf(); // Release�ϸ� nullptr�� �ʱ�ȭ���� �˾Ƽ� ����
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
	// rawSize�� scale = 1�� ���� �⺻ ����� �ǹ��ϹǷ� �׷����� ������ �������� ������ ���� ������Ʈ�� scale ���� ����ؾ� �Ѵ�. (ī�޶��� �������� ������Ʈ�� ������ǥ�� ũ�⸦ �����Ű�� ����)
	Vector3 worldPos = GetOwner()->transform->GetPosition();
	Vector3 scale = GetOwner()->transform->GetScale();
	float rotation = GetOwner()->transform->GetRotation().z;
	rotation -= Camera::GetMainCamera()->rotation; // ī�޶� �ð�������� ���� ������Ʈ�� �ݽð�������� �� �� ó�� �׷�����.
	float halfWidth = rawSize.x * scale.x / 2.f;
	float halfHeight = rawSize.y * scale.y / 2.f;

	// �� �������� ���� ��ǥ
	Vector3 lb{ -halfWidth, -halfHeight, 0 };
	Vector3 lt{ -halfWidth, halfHeight, 0 };
	Vector3 rt{ halfWidth, halfHeight, 0 };
	Vector3 rb{ halfWidth, -halfHeight, 0 };

	// ȸ�� �� ��� �̸� ����ؾ� �Ѵ�.
	if (!Math::Approximate(rotation, 0.0f))
	{
		float radian = Math::Deg2Rad(rotation);
		lb = lb.Rotate(radian);
		lt = lt.Rotate(radian);
		rt = rt.Rotate(radian);
		rb = rb.Rotate(radian);
	}

	// �ٿ�� �ڽ��� ���ϰ� ������ǥ�� ��ȯ�ؼ� ��ȯ
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