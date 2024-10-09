#include <string>
#include <iostream>
#include "DirectXFramework.h"
#include "ImageManager.h"
#include "Core.h"

NAMESPACE_DOGEENGINE_BEGIN

ComPtr<ID2D1Bitmap> ImageManager::GetD2DBitmapFromFile(std::wstring path, float quality)
{
    ComPtr<IWICImagingFactory2> wicFactory = DG::Core::GetInstance().wicFactory;
    //IWICImagingFactory* wicFactory = Core::Core::GetInstance().wicFactory;
    ComPtr<ID2D1DeviceContext> deviceContext = DG::Core::GetInstance().mainRenderer->GetDeviceContext();
    ComPtr<IWICBitmapDecoder> decoder = nullptr;

    HRESULT result = wicFactory->CreateDecoderFromFilename(path.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
    if (FAILED(result))
    {
        std::cerr << "failed to create decoder: " << std::hex << result << "\n";
    }

    // GIF 등 여러 프레임으로 구성된 경우를 고려
    ComPtr<IWICBitmapFrameDecode> frame = nullptr;
    decoder->GetFrame(0, frame.GetAddressOf()); // 첫 번째 프레임 선택


    if (quality == 0.8f)
    {
        int a = 10;
    }

    ComPtr<IWICFormatConverter> converter = nullptr;
    if (quality >= 0.9999f)
    {
        // WIC 포맷 -> D2D비트맵 포맷으로 변환하기 위해 컨버터 생성
        wicFactory->CreateFormatConverter(converter.GetAddressOf());
        converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
    }
    else
    {
        // lower resolution image
        UINT originalWidth, originalHeight;
        result = frame->GetSize(&originalWidth, &originalHeight);
        UINT newWidth = originalWidth * quality;
        UINT newHeight = originalHeight * quality;

        ComPtr<IWICBitmapScaler> scaler = nullptr;
        wicFactory->CreateBitmapScaler(scaler.GetAddressOf());
        scaler->Initialize(frame.Get(), newWidth, newHeight, WICBitmapInterpolationModeFant);

        wicFactory->CreateFormatConverter(converter.GetAddressOf());
        converter->Initialize(scaler.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
        // 이후 converter->Initialize에서 frame.get 쓰지 말고 scaler.get() 쓰면 됨
    }
    



    // WIC 포맷 -> D2D 비트맵으로 변환
    //ID2D1Bitmap* bitmap = nullptr;
    ComPtr<ID2D1Bitmap> bitmap;
    deviceContext->CreateBitmapFromWicBitmap(converter.Get(), NULL, bitmap.GetAddressOf());

    // 생성한 객체 릴리즈
    /*SafeRelease(&decoder);
    SafeRelease(&frame);
    SafeRelease(&converter);*/

    return bitmap;
}
std::vector<ComPtr<ID2D1Bitmap>> ImageManager::GetD2DBitmapsFromGIFFile(std::wstring path)
{
    std::vector<ComPtr<ID2D1Bitmap>> bitmaps;

    ComPtr<IWICImagingFactory2> wicFactory = DG::Core::GetInstance().wicFactory;
    //IWICImagingFactory* wicFactory = Core::Core::GetInstance().wicFactory;
    ComPtr<ID2D1DeviceContext> deviceContext = DG::Core::GetInstance().mainRenderer->GetDeviceContext();
    ComPtr<IWICBitmapDecoder> decoder = nullptr;

    wicFactory->CreateDecoderFromFilename(path.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());

    // WIC 포맷 -> D2D비트맵 포맷으로 변환하기 위해 컨버터 생성
    // IWICFormatConverter는 매번 CreateFormatConverter()로 새로 만들어줘야 한다??? Initialize() 자체가 한 번만 가능한 걸로 보임.
    ComPtr<IWICFormatConverter> converter = nullptr;

    UINT frameNum;
    decoder->GetFrameCount(&frameNum);
    for (int i = 0; i < frameNum; ++i)
    {
        ComPtr<IWICBitmapFrameDecode> frame = nullptr;
        decoder->GetFrame(i, frame.GetAddressOf());

        wicFactory->CreateFormatConverter(converter.GetAddressOf());
        converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);

        // WIC 포맷 -> D2D 비트맵으로 변환
        //ID2D1Bitmap* bitmap = nullptr;
        ComPtr<ID2D1Bitmap> bitmap;
        deviceContext->CreateBitmapFromWicBitmap(converter.Get(), NULL, bitmap.GetAddressOf());

        converter.ReleaseAndGetAddressOf();

        bitmaps.push_back(bitmap);
    }

    return bitmaps;
}

NAMESPACE_DOGEENGINE_END