#include "EffectManager.h"
#include "Core.h"

NAMESPACE_DOGEENGINE_BEGIN

ComPtr<ID2D1Effect> EffectManager::GetColorMatrixEffect(Color color)
{
    ComPtr<ID2D1Effect> colorMatrixEffect;
    ComPtr<ID2D1DeviceContext> deviceContext = DG::Core::GetInstance().GetMainRenderer()->GetDeviceContext();

    deviceContext->CreateEffect(CLSID_D2D1ColorMatrix, &colorMatrixEffect);
    D2D1_MATRIX_5X4_F matrix = D2D1::Matrix5x4F(
        color.R, 0, 0, 0,
        0, color.G, 0, 0,
        0, 0, color.B, 0,
        0, 0, 0, color.A,
        0, 0, 0, 0
    );
    colorMatrixEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, matrix); // 색조 매트릭스 적용
    colorMatrixEffect->SetValue(D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT, TRUE); // 0 ~ 1 클램핑

    return colorMatrixEffect;
}

NAMESPACE_DOGEENGINE_END