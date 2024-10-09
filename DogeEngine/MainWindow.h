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
	// �ձ� �𼭸����� �������� �������� ��簰�� �� ��κ� Desktop Window Manager�� ���� �̷������ �̰� â�� ������ �Ŀ� ����Ǳ� ������ �Ӽ������� ������ �Ұ����ϴ�.


	ATOM MyRegisterClass(HINSTANCE hInstance, LPCWSTR className);
	BOOL InitInstance(HINSTANCE hInstance, LPCWSTR title, LPCWSTR className);
	static LRESULT CALLBACK WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
public:
	static HINSTANCE CLIENT_INSTANCE;

	MainWindow(HINSTANCE hInstance, LPCWSTR title, LPCWSTR className, float initSizeX, float initSizeY);

	// GetClientRect�� ������� ���� �־ ScreenRect��
	RectF GetScreenRect();
	const wchar_t* GetWindowName();
	const wchar_t* GetWindowClassName();
	HWND GetHWND();

	void UpdateClientRect();
	void SetScreenSize(int width, int height);
};

NAMESPACE_DOGEENGINE_END