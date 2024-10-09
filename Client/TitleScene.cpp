#include <thread>
#include "TitleScene.h"
#include "ResourceLoader.h"
#include "LoadingHorse.h"
#include "TrashAnimation.h"
#include "Cursor_Aim.h"
#include <mfmediaengine.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mferror.h>
#include <mfidl.h>
#include <windows.h>
#include <iostream>
#include <filesystem>
#include <dcomp.h>
#include "SampleScene.h"
#include "GameState.h"
#pragma comment ( lib, "mfplat.lib")
#pragma comment ( lib, "Mf.lib")
#pragma comment ( lib, "dcomp.lib")

std::atomic<bool> load_complete = false;
std::atomic<bool> Video_complete = false;

class LicenseLogo : public Object
{
	void Awake() override
	{
		ResourceManager::AddImageByPath("Licese/FModLogo", L"Resource/UI/FModLogo.png");
		SpriteRenderer* sprite = AddComponent<SpriteRenderer>();
		sprite->SetImage("Licese/FModLogo", true);
		sprite->SetSizeByWidth(200.0f);

		transform->SetPosition(Camera::GetMainCamera()->width * 0.5f - sprite->GetWidth() * 0.5f - 50.0f, -Camera::GetMainCamera()->height * 0.5f + sprite->GetHeight() * 0.5f + 50.0f, 0.0f);
	}

	void Update() override
	{
	}

	void OnDestroy() override
	{
		ResourceManager::DeleteImage("Licese/FModLogo");
	}



};

class MediaEngineNotify : public IMFMediaEngineNotify
{
private:
	std::atomic<ULONG> refCount; // 참조 카운트를 위한 변수

public:
	MediaEngineNotify() : refCount(1) {} // 초기화

	// IMFMediaEngineNotify을(를) 통해 상속됨
	HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override
	{
		if (__uuidof(IMFMediaEngineNotify) == riid)
		{
			*ppvObject = static_cast<IMFMediaEngineNotify*>(this);
			AddRef(); // 참조 카운트 증가
			return S_OK;
		}

		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef(void) override
	{
		return refCount.fetch_add(1) + 1; // 참조 카운트를 증가시키고 반환
	}

	ULONG __stdcall Release(void) override
	{
		ULONG newCount = refCount.fetch_sub(1) - 1; // 참조 카운트를 감소시키고 반환
		if (newCount == 0)
		{
			delete this; // 참조 카운트가 0이 되면 객체를 삭제
		}
		return newCount;
	}

	HRESULT __stdcall EventNotify(DWORD event, DWORD_PTR param1, DWORD param2) override
	{
		// 이벤트 처리 로직을 여기에 추가
		switch (event)
		{
		case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
			break;
		case MF_MEDIA_ENGINE_EVENT_ERROR:
			// 오류 발생 시 처리
			break;
			// 추가 이벤트 케이스를 여기에 추가할 수 있습니다.
		default:
			break;
		}
		return S_OK; // 성공적으로 이벤트를 처리한 경우
	}
};
class IntroVideo : public Object
{

	void Awake() override
	{
		HRESULT result = MFStartup(MF_VERSION);

		ComPtr<IMFMediaEngineClassFactory> mediaEngineFactory;
		ComPtr<IMFMediaEngineNotify > unknown = new MediaEngineNotify();

		// 영상위에 비트맵 뛰우고싶으면 주석한게 맞는거같은데 잘모르겟음
		// ID3D11VideoDecoder,
		// ID3D11VideoContext 이거 일듯...
		// 다음 도지엔진, 혹은 지금넣으면 어떨까 싶음
		// https://learn.microsoft.com/ko-kr/windows/win32/medfound/supporting-direct3d-11-video-decoding-in-media-foundation
		
		result = DCompositionCreateDevice(DG::Core::GetInstance().GetMainRenderer()->GetDxgiDevice().Get(), __uuidof(IDCompositionDevice), (void**)&dcompDevice);
		
		result = dcompDevice->CreateTargetForHwnd(DG::Core::GetInstance().GetWindowHandle(), false, &dCompositionTarget);
		result = dcompDevice->CreateVisual(&dCompositionVisual);
		result = dCompositionTarget->SetRoot(dCompositionVisual.Get());

		//CreateTargetForHwnd로 만든 dCompositionTarget의 사이즈가 640.0f, 480.0f....
		result = dCompositionVisual->SetTransform(D2D1::Matrix3x2F::Scale(1920.0f / 640.0f, 1080.0f / 480.0f));
		



		result = MFCreateAttributes(&attributes, 0); 
		result = attributes->SetUnknown(MF_MEDIA_ENGINE_PLAYBACK_VISUAL, dCompositionVisual.Get());
		//result = attributes->SetUINT64(MF_MEDIA_ENGINE_PLAYBACK_HWND, (UINT64)DG::Core::GetInstance().GetWindowHandle());
		////result = attributes->SetUINT64(MF_MEDIA_ENGINE_OPM_HWND, (UINT64)DG::Core::GetInstance().GetWindowHandle());
		////result = attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT_B8G8R8A8_UNORM);
		result = attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, unknown.Get());


		result = CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&mediaEngineFactory));
		result = mediaEngineFactory->CreateInstance(MF_MEDIA_ENGINE_REAL_TIME_MODE | MF_MEDIA_ENGINE_DISABLE_LOCAL_PLUGINS, attributes.Get(), &mediaEngine);
		wchar_t url[] = L"Resource/Video/Intro.mp4";
		result = mediaEngine->SetSource(url);
		result = mediaEngine->Load();
		result = mediaEngine->Play();
	}

	void Update() override
	{
		if (dcompDevice)
		{
			dcompDevice->Commit();
		}
		bool isCreditEnd = mediaEngine->GetCurrentTime() >= mediaEngine->GetDuration();


		if (Input::IsDown(KeyCode::Esc) || Input::IsDown(KeyCode::Enter) || isCreditEnd)
		{
			Kill();
		}
	}

	void OnDestroy() override
	{
		HRESULT result;


		attributes->DeleteAllItems();

		result = mediaEngine->Shutdown();
		attributes.ReleaseAndGetAddressOf();
		mediaEngine.ReleaseAndGetAddressOf();


		Video_complete = true;
		result = MFShutdown();

	}
	ComPtr<IDCompositionDevice> dcompDevice;
	ComPtr<IDCompositionTarget> dCompositionTarget;
	ComPtr<IDCompositionVisual> dCompositionVisual;
	ComPtr<IMFAttributes> attributes;
	ComPtr<IMFMediaEngine> mediaEngine;

};


void TitleScene::Initialize()
{
	AddAndSetMainCameraObject();

	std::thread resourceThread(&TitleScene::LoadResource, this);
	resourceThread.detach();

	LoadingHorse* horse = AddObject<LoadingHorse>();

	AddUIObject<LicenseLogo>();

	//Video_complete = true;
	intro = AddObject<IntroVideo>();

	if (GetObjectByName("GameState") == nullptr)
	{
		AddObject<GameState>("GameState");
	}


		//
	/*TrashAnimationLoder* trainInside;
	TrashAnimationLoder* trainOutside;
	TrashAnimationLoder* train;
	TrashAnimationLoder* trainWheel;

	trainInside = AddObject<TrashAnimationLoder>();
	trainInside->path = L"Resource/Stage/train_wall_inside/";
	trainInside->key = "train_wall_in";
	trainInside->totalTime = 0.0f;
	trainInside->isloop = false;
	trainInside->sprite->enable = false;


	trainOutside = AddObject<TrashAnimationLoder>();
	trainOutside->path = L"Resource/Stage/train_wall_outside/";
	trainOutside->key = "train_wall_out";
	trainOutside->totalTime = 0.0f;
	trainOutside->isloop = false;
	trainOutside->sprite->enable = false;


	trainWheel = AddObject<TrashAnimationLoder>();
	trainWheel->path = L"Resource/Stage/wheel_come/";
	trainWheel->key = "wheel_come";
	trainWheel->totalTime = 0.0f;
	trainWheel->isloop = false;
	trainWheel->sprite->enable = false;

	train = AddObject<TrashAnimationLoder>();
	train->path = L"Resource/Stage/train_main_come/";
	train->key = "train_main_come";
	train->totalTime = 0.0f;
	train->isloop = false;
	train->SetDrawingPriority(OrderLayer::Train);
	train->sprite->enable = false;

	trainWheel = AddObject<TrashAnimationLoder>();
	trainWheel->path = L"Resource/Stage/wheel/";
	trainWheel->key = "wheel";
	trainWheel->totalTime = 0.0f;
	trainWheel->isloop = false;
	trainWheel->sprite->enable = false;
	trainWheel->quality = 0.8f;

	train = AddObject<TrashAnimationLoder>();
	train->path = L"Resource/Stage/train_main/";
	train->key = "train_main";
	train->totalTime = 0.0f;
	train->isloop = false;
	train->SetDrawingPriority(OrderLayer::Train);
	train->sprite->enable = false;

	trainWheel = AddObject<TrashAnimationLoder>();
	trainWheel->path = L"Resource/Stage/wheel_out/";
	trainWheel->key = "wheel_out";
	trainWheel->totalTime = 0.0f;
	trainWheel->isloop = false;
	trainWheel->sprite->enable = false;

	train = AddObject<TrashAnimationLoder>();
	train->path = L"Resource/Stage/train_main_out/";
	train->key = "train_main_out";
	train->totalTime = 0.0f;
	train->isloop = false;
	train->SetDrawingPriority(OrderLayer::Train);
	train->sprite->enable = false;*/

}

void listFiles(const std::filesystem::path& path, std::vector<std::filesystem::path>& pathVector)
{
	if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (entry.is_regular_file())
			{
				pathVector.push_back(entry.path());
			}
		}
	}
}
void TitleScene::LoadResource()
{
	ResourceLoader::LoadResource("Resource/resources.csv");

	//ResourceManager::AddNineSlicedImageByPath("sample", L"Resource/UI/9sliceSample.jpg", 54, 54, 54, 54);
	std::shared_ptr<Animation> anim_idle = ResourceManager::AddAnimation("rush/idle");
	anim_idle.get()->AddGIF("rushEnemy/idle");
	anim_idle.get()->loop = true;

 
	anim_idle = ResourceManager::AddAnimation("melee/idle");
	anim_idle.get()->AddGIF("meleeEnemy/idle");
	anim_idle.get()->loop = true;

	anim_idle = ResourceManager::AddAnimation("range/idle");
	anim_idle.get()->AddGIF("rangeEnemy/idle");
	anim_idle.get()->loop = true;

	std::shared_ptr<Animation> anim_attackReady = ResourceManager::AddAnimation("rush/attackReady");
	anim_attackReady.get()->AddGIF("rushEnemy/attackReady");
	anim_attackReady.get()->loop = true;


	anim_attackReady = ResourceManager::AddAnimation("melee/attackReady");
	anim_attackReady.get()->AddGIF("meleeEnemy/attackReady");
	anim_attackReady.get()->loop = true;

	anim_attackReady = ResourceManager::AddAnimation("range/attackReady");
	anim_attackReady.get()->AddGIF("rangeEnemy/attackReady");
	anim_attackReady.get()->loop = true;

	ResourceManager::AddSoundBankByPath("Sound", L"Resource/Sound/SoundEffect.fsb", L"Resource/Sound/SoundEffect.h");

	ResourceManager::AddAudioBankByPath("Master", L"Resource/Master.bank");
	ResourceManager::AddAudioBankByPath("Master.strings", L"Resource/Master.strings.bank");
	ResourceManager::AddAudioBankByPath("SFX", L"Resource/SFX.bank");


	ResourceManager::AddNineSlicedImageByPath("UI/Popup", L"Resource/UI/UI_Popup.png", 227, 235, 193, 280);

	ResourceManager::AddNineSlicedImageByPath("Tutorial/MessageBackground", L"Resource/UI/UI_Popup_Plain.png", 36, 36, 20, 20);




	auto trainHeart = ResourceManager::AddAnimation("TrainHeart");
	for (int i = 1; i <= 30; ++i)
	{
		std::string key = "TrainHeart/Idle/" + std::to_string(i);
		std::wstring path = L"Resource/Character/Heart/Img_TrainHeart_png/heart (" + std::to_wstring(i) + L").png";

		ResourceManager::AddImageByPath(key.c_str(), path.c_str());
		trainHeart->AddFrame(key.c_str());
	}
	trainHeart->loop = true;
	trainHeart->SetInterval(0.025f);

	int threadSize = std::thread::hardware_concurrency();
	std::vector<std::filesystem::path> temp;
	listFiles("Resource/Stage/", temp);
	for (auto& i : temp)
	{
		while (ResourceManager::GetLoadThreadCount() > 2 * threadSize + 1)
		{
			std::this_thread::yield();
		}

		if (i.extension() != ".png") continue;

		std::string key;
		std::wstring wkey = (--i.end())->c_str();
		key.assign(wkey.begin(), wkey.end());

		ResourceManager::AddImageByPathThread(key, i.c_str());
		//ResourceManager::DeleteImage(key);
	}

	while (!load_complete)
	{
		load_complete = ResourceManager::IsLoadComplete();
		std::this_thread::yield();
	}
	printf("로드 끝\n");
}
