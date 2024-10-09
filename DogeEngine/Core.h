#pragma once
#include "DirectXFramework.h"
#include "AudioFramework.h"
#include "defines.h"
#include "MainWindow.h"
#include "Time.h"
#include "Scene.h"
#include "Renderer.h"

class DrawingManager;
class ResourceManager;
class AudioManager;
class ResourceManager;
NAMESPACE_DOGEENGINE_BEGIN
class ImageManager;
class EffectManager;
class D2DRenderer;

class Core
{
private:
	static constexpr float fixedUpdateInterval = 0.0166667f;
	static constexpr float swapChainUpdateInterval = 0.005f; // 백버퍼 전환 간격

	bool KeepGoing = true;

	float initClientWidth = 1600;
	float initClientHeight = 900;

	float elapsedTimeForFixedUpdate = 0.0f;
	float elapsedTimeForDrawing = 0.0f;

	MainWindow* mainWindow;
	Time& timeManager;
	Scene* currentScene;
	// Update 중에 currentScene을 갈아끼우면 문제가 생길 수 있다. LoadScene이 호출되면 nextScene에 바꿀 신을 넣어놓고, 게임 루프의 마지막에 체크하여 갈아끼우도록 한다.
	Scene* nextScene = nullptr;
	ResourceManager* resurceManager;
	Core();
	~Core();
public:
	// ----------------- Initialize -----------------
	static Core& GetInstance();
	static Core& Initialize(HINSTANCE hInstance, LPCWSTR title, LPCWSTR mainWindowClassName);
	void Reset();

	// ----------------- Loop Control -----------------
	bool debug_mode = false;
	bool pause = false;
	bool onFocus = true;
	void Pause();
	void Resume();
	void ShutDownGame();
	void MainGameLoop(); // 메인 게임 루프

	// ----------------- Scene -----------------
	Scene* GetCurrentScene();
	Scene* GetNextScene();
	void SetCurrentScene(Scene* scene);
	void SetNextScene(Scene* scene);

	// ----------------- Main Loop -----------------
	void RunMainMessageLoop();
	void Update();
	void FixedUpdate();
	void LateUpdate();
	void Draw(); // Draw 단계에서 그리는 거랑 Update함수류에서 그리는 거랑 분리
	void EndDraw();

	// ----------------- Screen -----------------
	MainWindow* GetMainWindow();
	void AfterScreenSizeUpdate();
	void SetScreenSize(int width, int height);
	void SetScreenPositionCenter();
	void OnLoseFocus(); // 창의 포커스를 잃었을 때 (WM_KILLFOCUS)
	void OnRecoverFocus(); // 창의 포커스를 얻었을 때 (WM_SETFOCUS)
	void OnEnterSizeMove(); // 창의 제목표시줄을 잡고 이동시키거나 테두리를 잡고 크기를 바꾸기 시작했을 때 (WM_ENTERSIZEMOVE)
	void OnExitSizeMove(); // 창의 이동, 크기 변경 동작을 멈췄을 때 (WM_EXITSIZEMOVE)
	void SetCursorState(bool show);

	// ----------------- Getter -----------------
	// GetClientRect는 윈도우즈가 쓰고 있어서 ScreenRect로...
	RectF GetScreenRect();
	HWND GetWindowHandle();



private:
	// ----------------- COM Components -----------------
	friend class DrawingManager;
	friend class DG::ImageManager;
	friend class DG::EffectManager;
	friend class DG::D2DRenderer;
	friend class ResourceManager;
	friend class Scene;
	friend class AudioManager;

	std::shared_ptr<D2DRenderer> mainRenderer = nullptr;
public:
	std::shared_ptr<D2DRenderer> GetMainRenderer();
private:

	// DirectWrite // IDWriteFactory는 Windows7일 때 출시한 초기버전, Windows 10 이후는 IDWriteFactory3, 최신버전 윈도우즈용으론 IDWriteFactory5를 사용
	ComPtr<IDWriteFactory3> dwFactory = nullptr;
	ComPtr<IDWriteTextFormat> dwDefaultFont = nullptr; // default 포맷
	// Windows Imaging Component
	// DirectX COM 객체들은 내부적으로 알아서 COM 초기화를 처리하는데, 이 놈은 CoInitialize()로 명시적으로 초기화를 해줘야 하는 것 같음. CoInitialize()를 하지 않으면 작동은 하는데 해제할 때 접근할 수 없는 메모리 오류가 뜬다.
	// 근데 ComPtr로 수명을 관리하면 이게 CoUninitialize() 호출된 다음에 해제가 되는지 또 접근 불가능한 메모리 오류가 뜸. 그래서 방법은 세 가지인데 1) 그냥 CoUnitialize()를 빼거나 / 2) 이 놈만 포인터로 관리하면서 CoUninit 전에 수동으로 Release 해주거나 / 3) ComPtr.ReleaseAndGetAddressOf(); 으로 스마트 포인터를 포인터처럼 수동으로 릴리즈해주는 것. 지금은 3번 채택
	//IWICImagingFactory* wicFactory = nullptr;
	ComPtr<IWICImagingFactory2> wicFactory = nullptr;


	// FMOD
	FMOD::System* fmod_system;
	FMOD::Studio::System* fmod_Studiosystem;
	int fmod_channel_count = 64;
	unsigned int version;
	void* fmod_extradriverdata;
public:
	FMOD::System* GetFMODSystem();
public:
	FMOD::Studio::System* GetFmodStudioSystem();
};

NAMESPACE_DOGEENGINE_END