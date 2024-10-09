#include <iostream>
#include "Renderer.h"
#include "Vector3.h"
#include "Transform.h"
#include "Camera.h"
#include "SpriteRenderer.h"
#include "Core.h"
#include "Time.h"
#include "Math.h"
#include "CollisionCheck.h"
#include "MatrixTransform.h"

NAMESPACE_DOGEENGINE_BEGIN

ComPtr<ID2D1Factory1> D2DRenderer::d2dFactory = nullptr;
ComPtr<IDXGIDevice> D2DRenderer::dxgiDevice = nullptr;

D2DRenderer::D2DRenderer(int width, int height, Camera* camera)
{
	// D2D Device ������ ���� �ʿ� / ����� �ʿ����� ����
	if (d2dFactory == nullptr)
	{
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf());
	}

	HRESULT result = S_OK;

	// Direct3D11 Device & DXGI SwapChain
	// DXGI SwapChain�� ������۸� ���� �����ִ� ��. �길 �ʿ��ѵ� �긦 ������� Direct3D11 Device�� �ʿ���.
	// ���� �������� SwapChain�� ���������, �� ���� �������� SwapChain�� ���� ����� �ٲ� �� �� ������ ����� �ٲٰ� �ű⿡ �˸´� ��Ʈ���� ���ϱ� ���ؼ��� �ʿ��ϴ�. ��Ʈ�ʸ� ����� �ٲ� ���� ���� ��.
	ComPtr<ID3D11Device> d3dDevice;
	UINT createDeviceFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D3D_DRIVER_TYPE driverType[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP };

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = DG::Core::GetInstance().GetWindowHandle();
	swapChainDesc.Windowed = TRUE;

	for (int i = 0; i < 2; ++i)
	{
		result = D3D11CreateDeviceAndSwapChain(nullptr, driverType[i], nullptr, createDeviceFlag, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc, dxgiSwapChain.GetAddressOf(), d3dDevice.GetAddressOf(), nullptr, nullptr);

		if (FAILED(result))
			std::cerr << "failed to create d3d device and swap chain (driverType[" << i << "]): " << std::hex << result << "\n";
	}

	// DXGI Device
	// DXGI Surface�� Swap Chain ������ ���� �ʿ� / ����� �ʿ����� ����
	if (dxgiDevice == nullptr)
	{
		result = d3dDevice->QueryInterface(IID_PPV_ARGS(dxgiDevice.GetAddressOf()));
		if (FAILED(result))
			std::cerr << "failed to query interface: " << std::hex << result << "\n";
	}

	// Direct2D 1.1 Device, DeviceContext
	result = d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice);
	if (FAILED(result))
		std::cerr << "failed to create DXGI device: " << std::hex << result << "\n";
	result = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext);
	if (FAILED(result))
		std::cerr << "failed to create D2D device context: " << std::hex << result << "\n";

	// DXGI Surface, ��¿� ��Ʈ��
	// dxgiSwapChain�� �����(0��)�� �˸���? DXGI Surface�� �����Ѵ�.
	result = dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));
	if (FAILED(result))
		std::cerr << "failed to get buffer from DXGI swap chain: " << std::hex << result << "\n";
	float dpi = (float)GetDpiForWindow(GetDesktopWindow());
	D2D1_BITMAP_PROPERTIES1 properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), dpi, dpi);
	deviceContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &properties, &outputBitmap);
	deviceContext->SetTarget(outputBitmap.Get());

	this->camera = camera;
	if (camera != nullptr)
	{
		camera->ChangeRect((float)width, (float)height);
	}
}
ComPtr<IDXGIDevice> D2DRenderer::GetDxgiDevice()
{
	return dxgiDevice;
}
ComPtr<ID2D1DeviceContext> D2DRenderer::GetDeviceContext()
{
	return deviceContext;
}
ComPtr<ID2D1Bitmap1> D2DRenderer::GetOutputBitmap()
{
	return outputBitmap;
}
void D2DRenderer::ChangeCamera(Camera* camera)
{
	this->camera = camera;
}
Camera* D2DRenderer::GetCamera()
{
	return camera;
}
ComPtr<IDXGISwapChain> D2DRenderer::GetDXGISwapChain()
{
	return dxgiSwapChain;
}
void D2DRenderer::BeginDraw()
{
	if (useVSync)
	{
		deviceContext->BeginDraw();
		deviceContext->Clear(D2D1::ColorF(0x773333));
	}
	else
	{
		elapsedTime += Time::GetRealDeltaTime();
		if (elapsedTime >= swapChainUpdateInterval)
		{
			elapsedTime -= swapChainUpdateInterval;
			draw = true;
		}

		if (draw == false) return;

		deviceContext->BeginDraw();
		deviceContext->Clear(D2D1::ColorF(0x773333));
	}
}
void D2DRenderer::UpdateRenderer()
{
	if (useVSync)
	{
		deviceContext->EndDraw();
		dxgiSwapChain->Present(1, 0);
	}
	else
	{
		if (draw == false) return;

		deviceContext->EndDraw();
		dxgiSwapChain->Present(0, 0);

		draw = false;
	}
}
void D2DRenderer::UpdateBitmapSize(int width, int height)
{
	// DXGI SwapChain->ResizeBuffers�� ����ü�� ��ü�� ���� ������ ��� ���ܾ����� �۵���.
	outputBitmap.ReleaseAndGetAddressOf();
	dxgiSurface.ReleaseAndGetAddressOf();
	deviceContext->SetTarget(nullptr);

	HRESULT result = S_OK;

	// DXGI SwapChain ���� ������ ����
	result = dxgiSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
	if (FAILED(result))
		std::cerr << "Resize DXGI SwapChain buffer failed: " << std::hex << result << std::endl;

	// DXGI surface ��ü
	result = dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(dxgiSurface.GetAddressOf()));
	if (FAILED(result))
		std::cerr << "Change DXGI surface failed: " << std::hex << result << std::endl;

	// ��¿� ��Ʈ�� ���Ƴ����
	float dpi = (float)GetDpiForWindow(GetDesktopWindow());
	D2D1_BITMAP_PROPERTIES1 properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), dpi, dpi);
	result = deviceContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &properties, &outputBitmap);
	if (FAILED(result))
		std::cerr << "Change output bitmap failed: " << std::hex << result << std::endl;
	deviceContext->SetTarget(outputBitmap.Get());

	if (camera != nullptr)
	{
		camera->ChangeRect((float)width, (float)height);
	}
}

void D2DRenderer::SetReplaceBitmapOnScreenSizeChanged(bool set)
{
	replace_bitmap_on_screen_size_change = set;
}

bool D2DRenderer::GetReplaceBitmapOnScreenSizeChanged() const
{
	return replace_bitmap_on_screen_size_change;
}





bool D2DRenderer::Culling(SpriteRenderer* renderer)
{
	if (useAABBCulling == true && Math::Approximate(camera->rotation, 0.0f))
	{
		RectF boundingBoxRect = renderer->GetBoundingBoxRect();
		Vector2 boundsMin{ boundingBoxRect.left, boundingBoxRect.bottom };
		Vector2 boundsMax{ boundingBoxRect.right, boundingBoxRect.top };
		if (!CollisionCheck::BetweenAxisAlignedBox(boundsMin, boundsMax, Camera::GetMainCamera()->GetCameraMinPosition(), Camera::GetMainCamera()->GetCameraMaxPosition()))
		{
			return false;
		}
	}
	else if (useOBBCulling == true)
	{
		// �̿ϼ�.
	}

	return true;
}
void D2DRenderer::DrawBitmap(ID2D1Bitmap* image, RectF rect, RectF sourceRect, float opacity, Transform* transform, SpriteRenderer* renderer, bool drawingUI)
{
	if (!useVSync) 	if (draw == false) return;

	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return;
		}
	}
	if (!Culling(renderer)) return;

	if (drawingUI == false)
	{
		MatrixTransform::SetWorldTransformMatrix(deviceContext.Get(), transform, camera);
	}
	else if (drawingUI == true)
	{
		MatrixTransform::SetUITransformMatrix(deviceContext.Get(), transform, camera);
	}

	deviceContext->DrawBitmap(image, rect.ToD2D1Rect(), opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, sourceRect.ToD2D1Rect());

	MatrixTransform::ClearMatrix(deviceContext.Get());
}
void D2DRenderer::DrawEffectedImage(ID2D1Effect* effectedImage, RectF rect, RectF sourceRect, Transform* transform, SpriteRenderer* renderer, bool drawingUI)
{
	if (!useVSync) 	if (draw == false) return;

	if (camera == nullptr)
	{
		camera = DG::Core::GetInstance().GetCurrentScene()->mainCamera;
		if (camera == nullptr)
		{
			Assert(false, "failed to find camera.");
			return;
		}
	}
	if (!Culling(renderer)) return;

	float drawingWidth = rect.right - rect.left;
	float drawingHeight = rect.bottom - rect.top;
	float imgWidth = sourceRect.right - sourceRect.left;
	float imgHeight = sourceRect.bottom - sourceRect.top;
	Vector2 additionalScaling{ drawingWidth / imgWidth, drawingHeight / imgHeight };

	if (drawingUI == false)
	{
		MatrixTransform::SetWorldTransformMatrixWithAdditionalScaling(deviceContext.Get(), transform, additionalScaling, camera);
	}
	else if (drawingUI == true)
	{
		MatrixTransform::SetUITransformMatrixWithAdditionalScaling(deviceContext.Get(), transform, additionalScaling, camera);
	}


	// ID2D1Effect ��ü�� �׸� ������ �����ؼ� ũ�⸦ ��ȯ�� �� ����. ���� ��ǥ�踦 �߰��� �����ϸ��ؼ� �׷��� ũ����� �����Ѵ�.
	// DrawImage�� targetOffset�� �׷��� ������ �»���̸�, �׷κ��� sourceRect��ŭ �׸��� �ȴ�. �� �� �׷��� ������ �»���� ��ǥ�谡 �߰������� �����ϸ��Ǹ鼭 ���� �ڸ��� ������Ƿ�, �̸� ����(�߰��� �����ϸ��� ������ ������)�Ͽ� ���� �ڸ��� ã�ƿ����� ���� �� �׸���.
	//D2D1_POINT_2F point{ rect.left / (additionalScaling.x / Camera::GetMainCamera()->scale), rect.top / (additionalScaling.y / Camera::GetMainCamera()->scale)};
	D2D1_POINT_2F point{ rect.left / additionalScaling.x, rect.top / additionalScaling.y };
	deviceContext->DrawImage(effectedImage, point, sourceRect.ToD2D1Rect());

	MatrixTransform::ClearMatrix(deviceContext.Get());
}
void D2DRenderer::DrawLine(Vector2 start, Vector2 end, Color color, float width, bool drawingUI)
{
	if (!useVSync) 	if (draw == false) return;

	ID2D1SolidColorBrush* brush = NULL;

	deviceContext->CreateSolidColorBrush(color.ConvertToD2DColorF(), &brush);

	// ������ �򰥸��µ�, SRTMatrix �ȿ��� y���� �����Ǵϱ� to ������ y���� start.y���� end.y�� ���ϸ� �� �ǰ� end.y���� start.y�� ���ؾ� �Ѵ�??
	D2D1_POINT_2F to{ end.x - start.x, start.y - end.y };

	if (!drawingUI)
	{
		MatrixTransform::SetWorldTransformMatrix(deviceContext.Get(), start.ToVector3(), Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}
	else
	{
		MatrixTransform::SetUITransformMatrix(deviceContext.Get(), start.ToVector3(), Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}

	deviceContext->DrawLine({ 0, 0 }, to, brush, width);

	MatrixTransform::ClearMatrix(deviceContext.Get());

	SafeRelease(&brush);
}
void D2DRenderer::DrawSquare(Vector2 min, Vector2 max, Color lineColor, float lineWidth, bool drawingUI)
{
	if (!useVSync) 	if (draw == false) return;

	ID2D1SolidColorBrush* brush = NULL;

	HRESULT result = deviceContext->CreateSolidColorBrush(lineColor.ConvertToD2DColorF(), &brush);
	if (FAILED(result))
	{
		std::cerr << "failed to create brush: " << std::hex << result << "\n";
	}

	if (!drawingUI)
	{
		MatrixTransform::SetWorldTransformMatrix(deviceContext.Get(), Vector3{ min.x, max.y, 0 }, Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}
	else
	{
		MatrixTransform::SetUITransformMatrix(deviceContext.Get(), Vector3{ min.x, max.y, 0 }, Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}

	RectF rect{ 0, 0, max.x - min.x, max.y - min.y };
	deviceContext->DrawRectangle(rect.ToD2D1Rect(), brush, lineWidth);

	MatrixTransform::ClearMatrix(deviceContext.Get());

	SafeRelease(&brush);
}
void D2DRenderer::FillSquare(Vector2 min, Vector2 max, Color color, bool drawingUI)
{
	if (!useVSync) 	if (draw == false) return;

	ID2D1SolidColorBrush* brush = NULL;

	deviceContext->CreateSolidColorBrush(color.ConvertToD2DColorF(), &brush);

	if (!drawingUI)
	{
		MatrixTransform::SetWorldTransformMatrix(deviceContext.Get(), Vector3{ min.x, max.y, 0 }, Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}
	else
	{
		MatrixTransform::SetUITransformMatrix(deviceContext.Get(), Vector3{ min.x, max.y, 0 }, Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}

	RectF rect{ 0, 0, max.x - min.x, max.y - min.y };
	deviceContext->FillRectangle(rect.ToD2D1Rect(), brush);

	MatrixTransform::ClearMatrix(deviceContext.Get());

	SafeRelease(&brush);
}
void D2DRenderer::DrawCircle(Vector2 center, float radius, Color lineColor, float lineWidth, bool drawingUI)
{
	if (!useVSync) 	if (draw == false) return;

	ID2D1SolidColorBrush* brush = NULL;

	deviceContext->CreateSolidColorBrush(lineColor.ConvertToD2DColorF(), &brush);

	if (!drawingUI)
	{
		MatrixTransform::SetWorldTransformMatrix(deviceContext.Get(), center.ToVector3(), Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}
	else
	{
		MatrixTransform::SetUITransformMatrix(deviceContext.Get(), center.ToVector3(), Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}

	D2D1_ELLIPSE ellipse{ {0, 0}, radius, radius };
	deviceContext->DrawEllipse(ellipse, brush, lineWidth);

	MatrixTransform::ClearMatrix(deviceContext.Get());

	SafeRelease(&brush);
}
void D2DRenderer::FillCircle(Vector2 center, float radius, Color color, bool drawingUI)
{
	if (!useVSync) 	if (draw == false) return;

	ID2D1SolidColorBrush* brush = NULL;

	deviceContext->CreateSolidColorBrush(color.ConvertToD2DColorF(), &brush);

	if (!drawingUI)
	{
		MatrixTransform::SetWorldTransformMatrix(deviceContext.Get(), center.ToVector3(), Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}
	else
	{
		MatrixTransform::SetUITransformMatrix(deviceContext.Get(), center.ToVector3(), Vector3{ 0, 0, 0 }, Vector3{ 1, 1, 1 }, camera);
	}

	D2D1_ELLIPSE ellipse{ {0, 0}, radius, radius };
	deviceContext->FillEllipse(ellipse, brush);

	MatrixTransform::ClearMatrix(deviceContext.Get());

	SafeRelease(&brush);
}

void D2DRenderer::PrintText(Vector3 leftTopAnchor, Vector3 rotation, Vector3 scale, const wchar_t* text, Color color, const wchar_t* fontName, ComPtr<IDWriteFontCollection1> fontCollection, float fontSize, TextAlignVertical alignVertical, TextAlignHorizontal alignHorizontal, TextWeight weight, TextStyle style, bool drawingUI)
{
	if (!useVSync) 	if (draw == false) return;
	ID2D1SolidColorBrush* brush = NULL;

	deviceContext->CreateSolidColorBrush(color.ConvertToD2DColorF(), &brush);

	if (!drawingUI)
		MatrixTransform::SetWorldTransformMatrix(deviceContext.Get(), leftTopAnchor, rotation, scale, camera);
	else
		MatrixTransform::SetUITransformMatrix(deviceContext.Get(), leftTopAnchor, rotation, scale, camera);

	
	DWRITE_FONT_WEIGHT fontWeight{};
	DWRITE_FONT_STYLE fontStyle{};
	switch (weight)
	{
	case TextWeight::Thin:
		fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_LIGHT;
		break;
	case TextWeight::Normal:
		fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL;
		break;
	case TextWeight::Bold:
		fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL;
		break;
	case TextWeight::HeavyBold:
		fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL;
		break;
	}
	switch (style)
	{
	case TextStyle::Normal:
		fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL;
		break;
	case TextStyle::Italic:
		fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_ITALIC;
		break;
	case TextStyle::Oblique:
		fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_OBLIQUE;
		break;
	}
	DWRITE_TEXT_ALIGNMENT fontAlignHorizontal{};
	DWRITE_PARAGRAPH_ALIGNMENT fontAlignVertical{};
	switch (alignHorizontal)
	{
	case TextAlignHorizontal::Left:
		fontAlignHorizontal = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING;
		break;
	case TextAlignHorizontal::Center:
		fontAlignHorizontal = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER;
		break;
	case TextAlignHorizontal::Right:
		fontAlignHorizontal = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING;
		break;
	}
	switch (alignVertical)
	{
	case TextAlignVertical::Top:
		fontAlignVertical = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		break;
	case TextAlignVertical::Center:
		fontAlignVertical = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		break;
	case TextAlignVertical::Bottom:
		fontAlignVertical = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
		break;
	}


	IDWriteTextFormat* dwFormat = NULL;
	HRESULT result;
	if (fontCollection == nullptr)
	{
		result = Core::GetInstance().dwFactory->CreateTextFormat(
			fontName, // font set�� �̸�
			NULL, // �ռ� �̸��� ���� font set�� ����ִ� font collection. NULL�̸� �����쿡 ����ִ� �⺻ ��Ʈ �ݷ��ǿ��� ã��
			fontWeight,
			fontStyle,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"", //locale
			&dwFormat
		);


	}
	else
	{
		result = Core::GetInstance().dwFactory->CreateTextFormat(
			fontName,
			fontCollection.Get(),
			fontWeight,
			fontStyle,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"",
			&dwFormat
		);
	}

	dwFormat->SetTextAlignment(fontAlignHorizontal);
	dwFormat->SetParagraphAlignment(fontAlignVertical);


	SizeF dcSize = deviceContext->GetSize();

	deviceContext->DrawTextW(text, wcslen(text), dwFormat,
		D2D1::RectF(0, 0, dcSize.width, dcSize.height), brush);

	MatrixTransform::ClearMatrix(deviceContext.Get());

	SafeRelease(&brush);
	SafeRelease(&dwFormat);
}

NAMESPACE_DOGEENGINE_END