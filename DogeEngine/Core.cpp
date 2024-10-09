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

	//// 왜 소멸자 호출 시점부터 이미 해제되어 있지?
	//SafeRelease(&wicFactory);

	wicFactory.ReleaseAndGetAddressOf();

	fmod_system->close(); // FMOD 객체들의 fmod_system 참조성 제거
	fmod_system->release(); // fmod_system 메모리 해제

	fmod_Studiosystem->unloadAll();
	fmod_Studiosystem->flushCommands();
	fmod_Studiosystem->release();

	delete resurceManager;

	CoUninitialize();



}


void Core::MainGameLoop()
{
	// 엔진 내부에서 실제 시간 측정에 쓰거나, 타이머를 쓸 때는 GetReal류 함수를,
	// 클라단에서는 시간 배속을 조절하거나, 멈춤 효과 & 포커스가 벗어났을 때 애니메이션 일시 정지 등을 구현할 수 있으므로 Get류 함수를 사용할 것.
	timeManager.UpdateTime();

	// 씬의 초기화(InitializeScene)는 현재 SceneManager->ApplySceneChange()에서 해주고 있음!

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

	Input::ClearKeyState(); // 스크롤 때문에 필요
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

// Initialize는 static 함수이므로 instance를 통해 접근해야 함.
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

	//instance.UpdateScreenSize(); // 이걸 여기에 왜 써놨지?


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

// 메인 메시지 루프의 핵심은 GetMessage, DispatchMessage임. 굳이 특정 창 클래스에 종속시킬 필요 없음.
void Core::RunMainMessageLoop()
{
	// 일반 메시지 루프 - GetMessage는 메시지가 담길 때까지 대기하므로 게임용으로 부적합
	//MSG msg;
	//while (GetMessage(&msg, nullptr, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}

	// GetForegroundWindow()로 현재 포커스된 윈도추 창의 핸들을 얻을 수 있음.
	// 다만 이걸 사용하지 않고 SET,KILL_FOCUS 윈도우 메시지를 사용할 것임.
	//HWND hwndForeground = GetForegroundWindow();
	// 게임 창과 비교
	//return (hwndForeground == g_hwndGameWindow);

	MSG msg;
	while (KeepGoing)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // 메시지가 있으면
		{
			//GetMessage(&msg, NULL, 0, 0); // 가져와서
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg); // 처리하고
			//MainGameLoop(); // 차이가 없나?
		}
		else // 없으면
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

	// update를 안 해주면 처음 주어진 채널을 할당 해제하지 못 해서 주어진 개수를 다 쓰면 그 다음부터 소리가 안 들린다
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
	// 비트맵만 늘리고 clientRect는 갱신하지 않는 경우
	// 이 때는 Input에서만 clientRect를 갱신해서 스크린<->월드좌표만 변환할 수 있으면 된다.
	if (mainRenderer == nullptr || mainRenderer->GetReplaceBitmapOnScreenSizeChanged() == false) return;



	mainWindow->UpdateClientRect();

	RectF clientRect = GetScreenRect();
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	// 메인 렌더러가 사용하는 메인 카메라
	if (currentScene != nullptr && currentScene->mainCamera != nullptr)
	{
		currentScene->mainCamera->ChangeRect(clientRect);
	}

	// 메인 렌더러
	mainRenderer->UpdateBitmapSize(width, height);
}

void Core::SetScreenSize(int width, int height)
{
	// 코드에 의해서가 아니라 윈도우 이벤트(예컨대 변두리를 잡고 늘린다던가)에 의해서 창의 크기가 바뀔 수 있기 때문에, WM_SIZE 이벤트에서 AfterScreenSizeUpdate()를 호출하게 함
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