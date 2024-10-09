#include "Engine.h"
#include "Client.h"


// <ToDo>
// - ������Ʈ delete?
// - Component/Camera�� ScreenToWorldPoint() ���� �� �־ �ɱ�? ī�޶� ������Ʈ Ŭ������ ���� ī�޶� Ŭ���� ���� ����


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    DG::Core& GM = DG::Core::Initialize(hInstance, L"DogeEngine", L"mainWindowClass");

    //GM.debug_mode = true;
    GM.SetScreenSize(1920, 1080);
    GM.SetScreenPositionCenter();
    GM.GetMainRenderer()->SetReplaceBitmapOnScreenSizeChanged(false);
    GM.SetCursorState(false);

    SceneManager::ForceLoadScene<TitleScene>();

    GM.RunMainMessageLoop();


    //#include <crtdbg.h>
    //_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    //_CrtDumpMemoryLeaks();
}