#include "MainWindow.h"
#include "Input.h"
#include "Core.h"


NAMESPACE_DOGEENGINE_BEGIN

HINSTANCE MainWindow::CLIENT_INSTANCE;

MainWindow::MainWindow(HINSTANCE _hInstance, LPCWSTR title, LPCWSTR className, float initSizeX, float initSizeY) : windowName(title), windowClassName(className), initWidth(initSizeX), initHeight(initSizeY)
{
    windowStyle = WS_POPUP;

    MyRegisterClass(_hInstance, className);
    InitInstance(_hInstance, title, className);
}

ATOM MainWindow::MyRegisterClass(HINSTANCE gameInstance, LPCWSTR className)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;

    // 윈도우 프로시저는 OS가 호출하는 놈으로, 인스턴스가 없어도 언제든 호출될 수 있어야 해서 반드시 static 함수여야 한다. ~ MainWindow 클래스 선언부에서 WndProc에 달린 static 떼면 안 담긴다
    // 아마 모든 CALLBACK 함수들은 그런 것 같다?? 아래에 About 함수를 담아가는 DialogBox 함수도 About CALLBACK 함수가 non-static이면 오류가 난다.
    wcex.lpfnWndProc = MainWindow::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = gameInstance;
    wcex.hIcon = LoadIcon(gameInstance, MAKEINTRESOURCE(IDI_DOGEENGINE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    //wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CATCHCATS); // 메뉴바는 필요 없음
    wcex.lpszMenuName = NULL;

    wcex.lpszClassName = className;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL MainWindow::InitInstance(HINSTANCE hInstance, LPCWSTR title, LPCWSTR className)
{
    RECT initRect{ 0, 0, 800, 600};
    AdjustWindowRect(&initRect, windowStyle, FALSE); // 왜 안 되지?
    hWnd = CreateWindowW(className, title, windowStyle,
        0, 0, initRect.right - initRect.left, initRect.bottom - initRect.top, nullptr, nullptr, hInstance, nullptr);
    GetClientRect(hWnd, &clientRect);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    return TRUE;
}


RectF MainWindow::GetScreenRect()
{
    return RectF{ (float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom };
}

const wchar_t* MainWindow::GetWindowName()
{
    return windowName;
}

const wchar_t* MainWindow::GetWindowClassName()
{
    return windowClassName;
}

HWND MainWindow::GetHWND()
{
    return hWnd;
}


void MainWindow::UpdateClientRect()
{
    GetClientRect(hWnd, &clientRect);
}
void MainWindow::SetScreenSize(int width, int height)
{
    // 현재 창 스타일 가져오기
    DWORD style = GetWindowLong(hWnd, GWL_STYLE);
    DWORD exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

    // GetClientRect는 클라이언트 영역의 크기를 반환하고 AdjustWindowRect는 거기에 비클라이언트 영역의 크기를 더해주는 놈(대입이 아니라 더하는 놈임에 주의)
    RECT windowRect{ 0, 0, width, height };
    AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);

    // 미세한 차이는 도대체 왜

    SetWindowPos(hWnd, NULL, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOMOVE | SWP_NOZORDER);
}


INT_PTR CALLBACK MainWindow::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}




LRESULT MainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // WM_MOVE 창의 제목표시줄을 잡고 이동할 때
    // WM_SIZE 창의 사이즈를 변경할 때

    // 프로시저 함수는 OS에 의해 비동기적으로 호출되는 걸로 보임
    switch (message)
    {
    case WM_SETFOCUS:
    {
        //DGCore::Core::GetInstance().Resume();
        // 이렇게 FOCUS류 메시지 이용하지 않고 GetFocus()로 현재 포커스가 있는 창의 핸들 가져와서 비교해도 된다고 함.

        DG::Core::GetInstance().OnRecoverFocus();
    }
    break;
    case WM_KILLFOCUS:
    {
        //DGCore::Core::GetInstance().Pause();

        DG::Core::GetInstance().OnLoseFocus();
    }
    break;
    case WM_ENTERSIZEMOVE:
    {
        DG::Core::GetInstance().OnEnterSizeMove();
    }
    break;
    case WM_EXITSIZEMOVE:
    {
        DG::Core::GetInstance().OnExitSizeMove();
    }
    break;
    /*case WM_CREATE:
    {
    }
    break;*/
    case WM_MOVE:
    {
        DG::Core::GetInstance().MainGameLoop();
    }
    break;
    case WM_SIZE:
    {
        DG::Core::GetInstance().AfterScreenSizeUpdate();
    }
    break;
    case WM_LBUTTONDOWN:
    {
        Input::KeyEventQueue.push_back(std::pair<char, KeyState>(1, KeyState::KEYDOWN));
    }
    break;
    case WM_RBUTTONDOWN:
    {
        Input::KeyEventQueue.push_back(std::pair<char, KeyState>(2, KeyState::KEYDOWN));
    }
    break;
    case WM_MBUTTONDOWN:
    {
        Input::KeyEventQueue.push_back(std::pair<char, KeyState>(4, KeyState::KEYDOWN));
    }
    break;
    case WM_KEYDOWN:
    {
        Input::KeyEventQueue.push_back(std::pair<char, KeyState>(wParam, KeyState::KEYDOWN));
    }
    break;
    case WM_LBUTTONUP:
    {
        Input::KeyEventQueue.push_back(std::pair<char, KeyState>(1, KeyState::KEYUP));
    }
    break;
    case WM_RBUTTONUP:
    {
        Input::KeyEventQueue.push_back(std::pair<char, KeyState>(2, KeyState::KEYUP));
    }
    break;
    case WM_MBUTTONUP:
    {
        Input::KeyEventQueue.push_back(std::pair<char, KeyState>(4, KeyState::KEYUP));
    }
    break;
    case WM_KEYUP:
    {
        Input::KeyEventQueue.push_back(std::pair<char, KeyState>(wParam, KeyState::KEYUP));
    }
    break;
    case WM_MOUSEWHEEL:
    {
        Input::SetScrollAmount(GET_WHEEL_DELTA_WPARAM(wParam));
    }
    //case WM_MOUSEMOVE:
    //{
    //    // 굳이 여기서 갱신 안 하고 GetCursorPos WinAPI 함수 쓰는 게 낫다
    //}
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(CLIENT_INSTANCE, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT: // 안 쓰면 DefWindowProc에서 알아서 무효화 영역 제거한다
    {
        // BeginPaint - EndPaint로 무효화 영역을 제거해줘야 창을 이동시킬 때 점등현상이 생기지 않고 그려짐
        // <24-06-17> 여전히 창의 사이즈를 바꿀 때 점등 현상이 생긴다
        /*PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        Core::Core::GetInstance().MainGameLoop();

        EndPaint(hWnd, &ps);*/

        ValidateRect(hWnd, NULL);
        DG::Core::GetInstance().MainGameLoop();
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

NAMESPACE_DOGEENGINE_END