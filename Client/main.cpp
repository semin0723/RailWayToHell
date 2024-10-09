#include "Engine.h"
#include "Client.h"


// <ToDo>
// - 컴포넌트 delete?
// - Component/Camera에 ScreenToWorldPoint() 같은 게 있어도 될까? 카메라 컴포넌트 클래스와 범용 카메라 클래스 구분 검토


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