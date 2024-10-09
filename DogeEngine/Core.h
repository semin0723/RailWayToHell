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
	static constexpr float swapChainUpdateInterval = 0.005f; // ����� ��ȯ ����

	bool KeepGoing = true;

	float initClientWidth = 1600;
	float initClientHeight = 900;

	float elapsedTimeForFixedUpdate = 0.0f;
	float elapsedTimeForDrawing = 0.0f;

	MainWindow* mainWindow;
	Time& timeManager;
	Scene* currentScene;
	// Update �߿� currentScene�� ���Ƴ���� ������ ���� �� �ִ�. LoadScene�� ȣ��Ǹ� nextScene�� �ٲ� ���� �־����, ���� ������ �������� üũ�Ͽ� ���Ƴ��쵵�� �Ѵ�.
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
	void MainGameLoop(); // ���� ���� ����

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
	void Draw(); // Draw �ܰ迡�� �׸��� �Ŷ� Update�Լ������� �׸��� �Ŷ� �и�
	void EndDraw();

	// ----------------- Screen -----------------
	MainWindow* GetMainWindow();
	void AfterScreenSizeUpdate();
	void SetScreenSize(int width, int height);
	void SetScreenPositionCenter();
	void OnLoseFocus(); // â�� ��Ŀ���� �Ҿ��� �� (WM_KILLFOCUS)
	void OnRecoverFocus(); // â�� ��Ŀ���� ����� �� (WM_SETFOCUS)
	void OnEnterSizeMove(); // â�� ����ǥ������ ��� �̵���Ű�ų� �׵θ��� ��� ũ�⸦ �ٲٱ� �������� �� (WM_ENTERSIZEMOVE)
	void OnExitSizeMove(); // â�� �̵�, ũ�� ���� ������ ������ �� (WM_EXITSIZEMOVE)
	void SetCursorState(bool show);

	// ----------------- Getter -----------------
	// GetClientRect�� ������� ���� �־ ScreenRect��...
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

	// DirectWrite // IDWriteFactory�� Windows7�� �� ����� �ʱ����, Windows 10 ���Ĵ� IDWriteFactory3, �ֽŹ��� ������������� IDWriteFactory5�� ���
	ComPtr<IDWriteFactory3> dwFactory = nullptr;
	ComPtr<IDWriteTextFormat> dwDefaultFont = nullptr; // default ����
	// Windows Imaging Component
	// DirectX COM ��ü���� ���������� �˾Ƽ� COM �ʱ�ȭ�� ó���ϴµ�, �� ���� CoInitialize()�� ��������� �ʱ�ȭ�� ����� �ϴ� �� ����. CoInitialize()�� ���� ������ �۵��� �ϴµ� ������ �� ������ �� ���� �޸� ������ ���.
	// �ٵ� ComPtr�� ������ �����ϸ� �̰� CoUninitialize() ȣ��� ������ ������ �Ǵ��� �� ���� �Ұ����� �޸� ������ ��. �׷��� ����� �� �����ε� 1) �׳� CoUnitialize()�� ���ų� / 2) �� �� �����ͷ� �����ϸ鼭 CoUninit ���� �������� Release ���ְų� / 3) ComPtr.ReleaseAndGetAddressOf(); ���� ����Ʈ �����͸� ������ó�� �������� ���������ִ� ��. ������ 3�� ä��
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