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
	// D2D Device 생성을 위해 필요 / 재생성 필요하지 않음
	if (d2dFactory == nullptr)
	{
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf());
	}

	HRESULT result = S_OK;

	// Direct3D11 Device & DXGI SwapChain
	// DXGI SwapChain은 더블버퍼링 등을 도와주는 놈. 얘만 필요한데 얘를 만들려면 Direct3D11 Device가 필요함.
	// 메인 렌더러의 SwapChain은 사용하지만, 그 외의 렌더러의 SwapChain은 단지 사이즈를 바꿀 때 백 버퍼의 사이즈를 바꾸고 거기에 알맞는 비트맵을 구하기 위해서만 필요하다. 비트맵만 사이즈를 바꿀 수는 없는 듯.
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
	// DXGI Surface와 Swap Chain 생성을 위해 필요 / 재생성 필요하지 않음
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

	// DXGI Surface, 출력용 비트맵
	// dxgiSwapChain의 백버퍼(0번)와 알맞은? DXGI Surface를 생성한다.
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
	// DXGI SwapChain->ResizeBuffers는 스왑체인 객체에 대한 참조가 모두 끊겨야지만 작동함.
	outputBitmap.ReleaseAndGetAddressOf();
	dxgiSurface.ReleaseAndGetAddressOf();
	deviceContext->SetTarget(nullptr);

	HRESULT result = S_OK;

	// DXGI SwapChain 버퍼 사이즈 변경
	result = dxgiSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
	if (FAILED(result))
		std::cerr << "Resize DXGI SwapChain buffer failed: " << std::hex << result << std::endl;

	// DXGI surface 교체
	result = dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(dxgiSurface.GetAddressOf()));
	if (FAILED(result))
		std::cerr << "Change DXGI surface failed: " << std::hex << result << std::endl;

	// 출력용 비트맵 갈아끼우기
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
		// 미완성.
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


	// ID2D1Effect 객체는 그릴 영역을 지정해서 크기를 변환할 수 없다. 따라서 좌표계를 추가로 스케일링해서 그려질 크기까지 조절한다.
	// DrawImage의 targetOffset은 그려질 영역의 좌상단이며, 그로부터 sourceRect만큼 그리게 된다. 이 때 그려질 영역의 좌상단이 좌표계가 추가적으로 스케일링되면서 원래 자리를 벗어났으므로, 이를 보정(추가로 스케일링한 값으로 나눈다)하여 원래 자리를 찾아오도록 만든 뒤 그린다.
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

	// 굉장히 헷갈리는데, SRTMatrix 안에서 y축이 반전되니까 to 벡터의 y축은 start.y에서 end.y로 향하면 안 되고 end.y에서 start.y로 향해야 한다??
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
			fontName, // font set의 이름
			NULL, // 앞서 이름을 적은 font set이 들어있는 font collection. NULL이면 윈도우에 깔려있는 기본 폰트 콜렉션에서 찾음
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