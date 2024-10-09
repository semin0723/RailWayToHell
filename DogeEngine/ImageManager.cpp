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

    // GIF �� ���� ���������� ������ ��츦 ���
    ComPtr<IWICBitmapFrameDecode> frame = nullptr;
    decoder->GetFrame(0, frame.GetAddressOf()); // ù ��° ������ ����


    if (quality == 0.8f)
    {
        int a = 10;
    }

    ComPtr<IWICFormatConverter> converter = nullptr;
    if (quality >= 0.9999f)
    {
        // WIC ���� -> D2D��Ʈ�� �������� ��ȯ�ϱ� ���� ������ ����
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
        // ���� converter->Initialize���� frame.get ���� ���� scaler.get() ���� ��
    }
    



    // WIC ���� -> D2D ��Ʈ������ ��ȯ
    //ID2D1Bitmap* bitmap = nullptr;
    ComPtr<ID2D1Bitmap> bitmap;
    deviceContext->CreateBitmapFromWicBitmap(converter.Get(), NULL, bitmap.GetAddressOf());

    // ������ ��ü ������
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

    // WIC ���� -> D2D��Ʈ�� �������� ��ȯ�ϱ� ���� ������ ����
    // IWICFormatConverter�� �Ź� CreateFormatConverter()�� ���� �������� �Ѵ�??? Initialize() ��ü�� �� ���� ������ �ɷ� ����.
    ComPtr<IWICFormatConverter> converter = nullptr;

    UINT frameNum;
    decoder->GetFrameCount(&frameNum);
    for (int i = 0; i < frameNum; ++i)
    {
        ComPtr<IWICBitmapFrameDecode> frame = nullptr;
        decoder->GetFrame(i, frame.GetAddressOf());

        wicFactory->CreateFormatConverter(converter.GetAddressOf());
        converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);

        // WIC ���� -> D2D ��Ʈ������ ��ȯ
        //ID2D1Bitmap* bitmap = nullptr;
        ComPtr<ID2D1Bitmap> bitmap;
        deviceContext->CreateBitmapFromWicBitmap(converter.Get(), NULL, bitmap.GetAddressOf());

        converter.ReleaseAndGetAddressOf();

        bitmaps.push_back(bitmap);
    }

    return bitmaps;
}

NAMESPACE_DOGEENGINE_END