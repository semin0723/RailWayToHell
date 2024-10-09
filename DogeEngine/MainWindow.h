#pragma once
#include "WinAPIFramework.h"
#include "defines.h"
#include "Structures.h"


NAMESPACE_DOGEENGINE_BEGIN

class MainWindow
{	
	const wchar_t* windowName;
	const wchar_t* windowClassName;

	HWND hWnd;
	RECT clientRect;
	int initWidth;
	int initHeight;

	long long defaultWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	long long windowStyle;
	// 둥근 모서리같이 윈도우의 세부적인 모양같은 건 대부분 Desktop Window Manager를 통해 이루어지며 이건 창이 생성된 후에 적용되기 때문에 속성값으로 변경이 불가능하다.


	ATOM MyRegisterClass(HINSTANCE hInstance, LPCWSTR className);
	BOOL InitInstance(HINSTANCE hInstance, LPCWSTR title, LPCWSTR className);
	static LRESULT CALLBACK WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
public:
	static HINSTANCE CLIENT_INSTANCE;

	MainWindow(HINSTANCE hInstance, LPCWSTR title, LPCWSTR className, float initSizeX, float initSizeY);

	// GetClientRect는 윈도우즈가 쓰고 있어서 ScreenRect로
	RectF GetScreenRect();
	const wchar_t* GetWindowName();
	const wchar_t* GetWindowClassName();
	HWND GetHWND();

	void UpdateClientRect();
	void SetScreenSize(int width, int height);
};

NAMESPACE_DOGEENGINE_END