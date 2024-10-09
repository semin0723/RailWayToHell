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

    // ������ ���ν����� OS�� ȣ���ϴ� ������, �ν��Ͻ��� ��� ������ ȣ��� �� �־�� �ؼ� �ݵ�� static �Լ����� �Ѵ�. ~ MainWindow Ŭ���� ����ο��� WndProc�� �޸� static ���� �� ����
    // �Ƹ� ��� CALLBACK �Լ����� �׷� �� ����?? �Ʒ��� About �Լ��� ��ư��� DialogBox �Լ��� About CALLBACK �Լ��� non-static�̸� ������ ����.
    wcex.lpfnWndProc = MainWindow::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = gameInstance;
    wcex.hIcon = LoadIcon(gameInstance, MAKEINTRESOURCE(IDI_DOGEENGINE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    //wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CATCHCATS); // �޴��ٴ� �ʿ� ����
    wcex.lpszMenuName = NULL;

    wcex.lpszClassName = className;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL MainWindow::InitInstance(HINSTANCE hInstance, LPCWSTR title, LPCWSTR className)
{
    RECT initRect{ 0, 0, 800, 600};
    AdjustWindowRect(&initRect, windowStyle, FALSE); // �� �� ����?
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
    // ���� â ��Ÿ�� ��������
    DWORD style = GetWindowLong(hWnd, GWL_STYLE);
    DWORD exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

    // GetClientRect�� Ŭ���̾�Ʈ ������ ũ�⸦ ��ȯ�ϰ� AdjustWindowRect�� �ű⿡ ��Ŭ���̾�Ʈ ������ ũ�⸦ �����ִ� ��(������ �ƴ϶� ���ϴ� ���ӿ� ����)
    RECT windowRect{ 0, 0, width, height };
    AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);

    // �̼��� ���̴� ����ü ��

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
    // WM_MOVE â�� ����ǥ������ ��� �̵��� ��
    // WM_SIZE â�� ����� ������ ��

    // ���ν��� �Լ��� OS�� ���� �񵿱������� ȣ��Ǵ� �ɷ� ����
    switch (message)
    {
    case WM_SETFOCUS:
    {
        //DGCore::Core::GetInstance().Resume();
        // �̷��� FOCUS�� �޽��� �̿����� �ʰ� GetFocus()�� ���� ��Ŀ���� �ִ� â�� �ڵ� �����ͼ� ���ص� �ȴٰ� ��.

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
    //    // ���� ���⼭ ���� �� �ϰ� GetCursorPos WinAPI �Լ� ���� �� ����
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
    case WM_PAINT: // �� ���� DefWindowProc���� �˾Ƽ� ��ȿȭ ���� �����Ѵ�
    {
        // BeginPaint - EndPaint�� ��ȿȭ ������ ��������� â�� �̵���ų �� ���������� ������ �ʰ� �׷���
        // <24-06-17> ������ â�� ����� �ٲ� �� ���� ������ �����
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