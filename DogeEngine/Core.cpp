#include "Core.h"
#include "StandardFramework.h"
#include "SceneManager.h"
#include "Scene.h"
#include "MainWindow.h"
#include "Renderer.h"
#include "Camera.h"
#include "DrawingManager.h"
#include "Structures.h"

#include "ResourceManager.h"

NAMESPACE_DOGEENGINE_BEGIN

Core::Core() : timeManager(Time::GetInstance())
{
	resurceManager = new ResourceManager();
}
Core::~Core()
{
	if (currentScene)
	{
		delete currentScene;
		currentScene = nullptr;
	}
	if (mainWindow)
	{
		delete mainWindow;
		mainWindow = nullptr;
	}

	//SafeRelease(&renderTarget);
	//SafeRelease(&d2dFactory);

	//SafeRelease(&dwFactory);
	//SafeRelease(&dwDefaultFont);

	//// �� �Ҹ��� ȣ�� �������� �̹� �����Ǿ� ����?
	//SafeRelease(&wicFactory);

	wicFactory.ReleaseAndGetAddressOf();

	fmod_system->close(); // FMOD ��ü���� fmod_system ������ ����
	fmod_system->release(); // fmod_system �޸� ����

	fmod_Studiosystem->unloadAll();
	fmod_Studiosystem->flushCommands();
	fmod_Studiosystem->release();

	delete resurceManager;

	CoUninitialize();



}


void Core::MainGameLoop()
{
	// ���� ���ο��� ���� �ð� ������ ���ų�, Ÿ�̸Ӹ� �� ���� GetReal�� �Լ���,
	// Ŭ��ܿ����� �ð� ����� �����ϰų�, ���� ȿ�� & ��Ŀ���� ����� �� �ִϸ��̼� �Ͻ� ���� ���� ������ �� �����Ƿ� Get�� �Լ��� ����� ��.
	timeManager.UpdateTime();

	// ���� �ʱ�ȭ(InitializeScene)�� ���� SceneManager->ApplySceneChange()���� ���ְ� ����!

	Draw();

	if (onFocus)
	{
		Input::UpdateKeyState();
	}

	Update();
	timeManager.updateCount++;

	elapsedTimeForFixedUpdate += Time::GetRealDeltaTime();
	if (elapsedTimeForFixedUpdate >= fixedUpdateInterval)
	{
		elapsedTimeForFixedUpdate -= fixedUpdateInterval;

		timeManager.UpdateFixedTime();
		FixedUpdate();
		timeManager.fixedUpdateCount++;
	}

	LateUpdate();

	EndDraw();

	if (nextScene != nullptr)
	{
		SceneManager::ApplySceneChange();
	}

	Input::ClearKeyState(); // ��ũ�� ������ �ʿ�
}
void Core::Pause()
{
	pause = true;
}
void Core::Resume()
{
	pause = false;
	timeManager.InitTime();
}

RectF Core::GetScreenRect()
{
	return mainWindow->GetScreenRect();
}

Scene* Core::GetCurrentScene()
{
	return currentScene;
}

Scene* Core::GetNextScene()
{
	return nextScene;
}

// Initialize�� static �Լ��̹Ƿ� instance�� ���� �����ؾ� ��.
Core& Core::Initialize(HINSTANCE hInstance, LPCWSTR title, LPCWSTR mainWindowClassName)
{
	HRESULT result = S_OK;

	Core& instance = Core::GetInstance();
	instance.currentScene = nullptr;
	instance.mainWindow = new MainWindow(hInstance, title, mainWindowClassName, instance.initClientWidth, instance.initClientHeight);

	// COM
	result = CoInitialize(NULL);
	if (FAILED(result))
	{
		std::cerr << "failed to CoInitialize: " << std::hex << result << "\n";
	}

	RectF clientRect = instance.mainWindow->GetScreenRect();
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	instance.mainRenderer = std::shared_ptr<D2DRenderer>(new D2DRenderer(width, height, nullptr));

	// DirectWrite1 -> for windows 7
	/*DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(dwFactory),
		reinterpret_cast<IUnknown**>(instance.dwFactory.GetAddressOf())
	);*/
	// DirectWrite over5 -> for windows 10 and later
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory3), &instance.dwFactory);
	//instance.dwFactory.As(&instance.dwFactory5);

	instance.dwFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,
		L"", //locale
		instance.dwDefaultFont.GetAddressOf()
	);

	// Windows Imaging Component
	CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&instance.wicFactory));

	//instance.UpdateScreenSize(); // �̰� ���⿡ �� �����?


	// FMOD
	FMOD_RESULT fmod_result;
	fmod_result = FMOD::System_Create(&instance.fmod_system);
	Assert(fmod_result == FMOD_OK, "failed to create fmod system.");
	//fmod_result = instance.fmod_system->getVersion(&instance.version);
	Assert(fmod_result == FMOD_OK, "failed to get version of fmod system");
	fmod_result = instance.fmod_system->init(instance.fmod_channel_count, FMOD_INIT_NORMAL, instance.fmod_extradriverdata);
	Assert(fmod_result == FMOD_OK, "failed to initialize fmod system.");




	fmod_result = FMOD::Studio::System::create(&instance.fmod_Studiosystem);
	Assert(fmod_result == FMOD_OK, "failed to create fmod Studio system.");
	fmod_result = instance.fmod_Studiosystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
	Assert(fmod_result == FMOD_OK, "failed to initialize fmod Studio system.");


	return instance;
}

void Core::Reset()
{
	elapsedTimeForFixedUpdate = 0;
}
Core& Core::GetInstance()
{
	static Core instance;
	return instance;
}

// ���� �޽��� ������ �ٽ��� GetMessage, DispatchMessage��. ���� Ư�� â Ŭ������ ���ӽ�ų �ʿ� ����.
void Core::RunMainMessageLoop()
{
	// �Ϲ� �޽��� ���� - GetMessage�� �޽����� ��� ������ ����ϹǷ� ���ӿ����� ������
	//MSG msg;
	//while (GetMessage(&msg, nullptr, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}

	// GetForegroundWindow()�� ���� ��Ŀ���� ������ â�� �ڵ��� ���� �� ����.
	// �ٸ� �̰� ������� �ʰ� SET,KILL_FOCUS ������ �޽����� ����� ����.
	//HWND hwndForeground = GetForegroundWindow();
	// ���� â�� ��
	//return (hwndForeground == g_hwndGameWindow);

	MSG msg;
	while (KeepGoing)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // �޽����� ������
		{
			//GetMessage(&msg, NULL, 0, 0); // �����ͼ�
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg); // ó���ϰ�
			//MainGameLoop(); // ���̰� ����?
		}
		else // ������
		{
			if (pause == true) continue;
			MainGameLoop();
		}
	}
}


void Core::SetCurrentScene(Scene* scene)
{
	currentScene = scene;
}
void Core::SetNextScene(Scene* scene)
{
	nextScene = scene;
}

HWND Core::GetWindowHandle()
{
	if (mainWindow == nullptr) return nullptr;
	return mainWindow->GetHWND();
}

std::shared_ptr<D2DRenderer> Core::GetMainRenderer()
{
	return mainRenderer;
}

FMOD::System* Core::GetFMODSystem()
{
	return fmod_system;
}

FMOD::Studio::System* Core::GetFmodStudioSystem()
{
	return fmod_Studiosystem;
}



void Core::Update()
{
	if (currentScene == nullptr) return;

	// update�� �� ���ָ� ó�� �־��� ä���� �Ҵ� �������� �� �ؼ� �־��� ������ �� ���� �� �������� �Ҹ��� �� �鸰��
	fmod_system->update();
	fmod_Studiosystem->update();

	currentScene->Update();
}
void Core::FixedUpdate()
{
	if (currentScene == nullptr) return;

	currentScene->FixedUpdate();
}
void Core::LateUpdate()
{
	if (currentScene == nullptr) return;

	currentScene->LateUpdate();
}


void Core::Draw()
{
	if (mainRenderer == nullptr || currentScene == nullptr) return;

	mainRenderer->BeginDraw();

	auto renderers = GetCurrentScene()->additionalRenderer;
	for (auto renderer : renderers)
	{
		if (renderer.second.get()->enabled == false) continue;
		renderer.second.get()->BeginDraw();
	}

	currentScene->Draw();

	if (debug_mode == true && currentScene->mainCamera != nullptr)
	{
		RectF clientRect = GetScreenRect();

		float width = Camera::GetMainCamera()->width / 2;
		float height = Camera::GetMainCamera()->height / 2;

		wchar_t* wc = new wchar_t[100];
		swprintf_s(wc, 100, L"screen size\nwidth: %.0f    height: %.0f", clientRect.right, clientRect.bottom);
		DrawingManager::PrintText(wc, Vector2{ -width + 10, height - 10 }, Color{ 1, 0, 0, 1.0f }, L"Arial", 24.0f, true);

		swprintf_s(wc, 100, L"updatePerSec: %d\nfixedUpdatePerSec: %d", (int)timeManager.GetUpdateCountPerSecond(), (int)timeManager.GetFixedUpdateCountPerSecond());
		DrawingManager::PrintText(wc, Vector2{ -width + 10, height - 110 }, Color{ 1, 0, 0, 1.0f }, L"Arial", 24.0f, true);

		Vector2 mousePos = Input::GetMousePos();
		Vector2 mousePosToWorldPos = Camera::ScreenToWorldPoint(Input::GetMousePos());
		swprintf_s(wc, 100, L"MousePos: %d, %d\nMousePosToWorldPos: %d, %d", (int)mousePos.x, (int)mousePos.y, (int)mousePosToWorldPos.x, (int)mousePosToWorldPos.y);
		DrawingManager::PrintText(wc, Vector2{ -width + 10, height - 210 }, Color{ 1, 0, 0, 1.0f }, L"Arial", 24.0f, true);

		delete[] wc;
	}
}

void Core::EndDraw()
{
	if (mainRenderer == nullptr || currentScene == nullptr) return;

	mainRenderer->UpdateRenderer();
	auto renderers = GetCurrentScene()->additionalRenderer;
	for (auto renderer : renderers)
	{
		if (renderer.second.get()->enabled == false) continue;
		renderer.second.get()->UpdateRenderer();
	}
}

void Core::ShutDownGame()
{
	KeepGoing = false;
}

MainWindow* Core::GetMainWindow()
{
	return mainWindow;
}


void Core::AfterScreenSizeUpdate()
{
	MainWindow* mainWindow = GetMainWindow();
	if (mainWindow == nullptr) return;
	


	Input::UpdateClientRect();
	// ��Ʈ�ʸ� �ø��� clientRect�� �������� �ʴ� ���
	// �� ���� Input������ clientRect�� �����ؼ� ��ũ��<->������ǥ�� ��ȯ�� �� ������ �ȴ�.
	if (mainRenderer == nullptr || mainRenderer->GetReplaceBitmapOnScreenSizeChanged() == false) return;



	mainWindow->UpdateClientRect();

	RectF clientRect = GetScreenRect();
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	// ���� �������� ����ϴ� ���� ī�޶�
	if (currentScene != nullptr && currentScene->mainCamera != nullptr)
	{
		currentScene->mainCamera->ChangeRect(clientRect);
	}

	// ���� ������
	mainRenderer->UpdateBitmapSize(width, height);
}

void Core::SetScreenSize(int width, int height)
{
	// �ڵ忡 ���ؼ��� �ƴ϶� ������ �̺�Ʈ(������ ���θ��� ��� �ø��ٴ���)�� ���ؼ� â�� ũ�Ⱑ �ٲ� �� �ֱ� ������, WM_SIZE �̺�Ʈ���� AfterScreenSizeUpdate()�� ȣ���ϰ� ��
	if (mainWindow != nullptr) mainWindow->SetScreenSize(width, height);
}
void Core::SetScreenPositionCenter()
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	RectF clientRect = mainWindow->GetScreenRect();

	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;

	SetWindowPos(GetWindowHandle(), NULL,
		screenWidth / 2 - clientWidth / 2,
		screenHeight / 2 - clientHeight / 2,
		clientWidth, clientHeight, 0
	);
}
void Core::OnLoseFocus()
{
	if (currentScene == nullptr) return;
	currentScene->OnLoseFocus();

	onFocus = false;
}
void Core::OnRecoverFocus()
{
	if (currentScene == nullptr) return;
	currentScene->OnRecoverFocus();

	onFocus = true;
}
void Core::OnEnterSizeMove()
{
	//Pause();

	if (currentScene == nullptr) return;
	currentScene->OnEnterSizeMove();
}
void Core::OnExitSizeMove()
{
	//Resume();

	if (currentScene == nullptr) return;
	currentScene->OnExitSizeMove();
}

void Core::SetCursorState(bool show)
{
	ShowCursor(show);
}

NAMESPACE_DOGEENGINE_END