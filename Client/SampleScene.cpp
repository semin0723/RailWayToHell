#include "SampleScene.h"
#include "GameManager.h"
#include "Cursor_Aim.h"
#include "TrashAnimation.h"
#include "ScrollObject.h"
#include "ButtonUI.h"
#include "PositionHelper.h"
#include "GameState.h"
#include "Config.h"

using namespace std::string_literals;

class GameSetting : public Object
{
	void Update() override;
};

SampleScene::~SampleScene()
{
	/*ResourceManager::DeleteAudioBank("SFX");
	ResourceManager::DeleteAudioBank("Master.strings");
	ResourceManager::DeleteAudioBank("Master");*/
}

void SampleScene::Initialize()
{
	AddAndSetMainCameraObject();

	GameState* gameState = GetObjectByName<GameState>("GameState");
	if (gameState == nullptr)
	{
		printf("please show me a mercy\n");
		gameState = AddObject<GameState>("GameState");
	}


	if (GetObjectByName("Cursor") == nullptr)
	{
		Cursor_Aim* aim = AddUIObject<Cursor_Aim>("Cursor");
		aim->SetDontDestroyOnSceneChange(true);
	}

	ScrollBackGround* backGround = GetObjectByName<ScrollBackGround>("ScrollBackGround");
	if (!backGround)
	{
		backGround = AddObject<ScrollBackGround>("ScrollBackGround");
	}
	backGround->SetBackGround("sunset");
	//AddObject<SceneEnterEffecter>();

	TrashAnimation* trainRail = GetObjectByName<TrashAnimation>("trainRail");
	if (!trainRail)
	{
		trainRail = AddObject<TrashAnimation>("trainRail");
		trainRail->path = L"Resource/Stage/rail/";
		trainRail->key = "rail";
		trainRail->totalTime = 0.1f;
		trainRail->isloop = true;
		trainRail->quality = 1.0f;
		trainRail->SetDrawingPriority(OrderLayer::TrainRail);
		trainRail->sprite->anchorType = AnchorType::BottomCenter;
		trainRail->transform->SetPositionY(-Camera::GetMainCamera()->height * 0.5f);
		trainRail->SetDontDestroyOnSceneChange(true);
	}

	// AddObject<GameSetting>();


	BackGroundMusic* backGroundMusic = GetObjectByName<BackGroundMusic>("BackGroundMusic");
	if (!backGroundMusic)
	{
		backGroundMusic = AddObject<BackGroundMusic>("BackGroundMusic");
	}
	if (gameState->isRestart == true)
	{
		GameManager* gameManager = GetObjectByName<GameManager>("GameManager");
		if (gameManager == nullptr)
		{
			gameManager = AddObject<GameManager>("GameManager");
		}
		gameState->isRestart = false;
		gameManager->LoadStageFile();

		return;
	}
	else
	{
		backGroundMusic->backGroundMsusic->Stop();
		delete backGroundMusic->backGroundMsusic;
		backGroundMusic->backGroundMsusic = ResourceManager::GetAudioBank("SFX")->Play("event:/Ambience/Western");
	}
	backGroundMusic->Play();


	// 타이틀 디자인
	TitleUI = AddUIObject<Object>();

	auto tempObject = AddUIObject<Object>();
	auto tempObjectSprite = tempObject->AddComponent<SpriteRenderer>();
	tempObjectSprite->SetImage("UI/Gradation");
	tempObjectSprite->SetSizeAuto();
	tempObject->transform->SetParent(TitleUI->transform);

	Object* title_logo;
	title_logo = AddUIObject<Object>();
	title_logo_renderer = title_logo->AddComponent<SpriteRenderer>();
	title_logo_renderer->SetImage("Title/Logo");
	title_logo_renderer->SetSizeAuto();
	title_logo->transform->SetPosition(-505, 214, 0);
	title_logo->transform->SetParent(TitleUI->transform);

	HolyButton* start_btn = AddObject<HolyButton>();
	start_btn->change_sprite_on_hover = true;
	start_btn->transform->SetPosition(-508, -193, 0);
	start_btn->transform->SetParent(TitleUI->transform);
	start_btn->RegisterSprite("UI/GameStartBtn", "UI/GameStartBtn/Selected");
	start_btn->clicked = [this, start_btn]()
		{
			AudioFramework::Play_BankSound("Sound", "UI_Click_Titlebtn");

			GameManager* gameManager = start_btn->GetOwner()->GetObjectByName<GameManager>("GameManager");
			if (gameManager == nullptr)
			{
				gameManager = start_btn->AddObject<GameManager>("GameManager");
			}
			gameManager->LoadStageFile();


			float title_ui_fade_out_duration = 0.2f;

			SampleScene* scene = static_cast<SampleScene*>(DG::Core::GetInstance().GetCurrentScene());
			scene->TitleUI->AddRealTimeTimerFunction([scene, title_ui_fade_out_duration](bool lu)
				{
					for (auto& item : *scene->TitleUI->transform->GetChild())
					{
						SpriteRenderer* sprite = item->GetOwner()->GetComponent<SpriteRenderer>();
						sprite->color.A -= Time::GetRealDeltaTime() / title_ui_fade_out_duration;
					}
					if (lu)
					{
						scene->TitleUI->Kill();
					}
				}, true, title_ui_fade_out_duration, 1);
		};

	HolyButton* quit_btn = AddObject<HolyButton>();
	quit_btn->change_sprite_on_hover = true;
	quit_btn->transform->SetPosition(-508, -350, 0);
	quit_btn->transform->SetParent(TitleUI->transform);
	quit_btn->RegisterSprite("UI/QuitBtn", "UI/QuitBtn/Selected");
	quit_btn->clicked = [this, quit_btn]()
		{
			AudioFramework::Play_BankSound("Sound", "UI_Click_Titlebtn");
			DG::Core::GetInstance().ShutDownGame();
		};

	/*PositionHelper* helper = AddObject<PositionHelper>();
	helper->AddTarget(title_logo);
	helper->AddTarget(start_btn);
	helper->AddTarget(quit_btn);*/





	for (auto& item : *TitleUI->transform->GetChild())
	{
		SpriteRenderer* sprite = item->GetOwner()->GetComponent<SpriteRenderer>();
		sprite->color.A = 0.0f;
	}
	TitleUI->AddRealTimeTimerFunction([this](bool lu)
		{
			for (auto& item : *TitleUI->transform->GetChild())
			{
				SpriteRenderer* sprite = item->GetOwner()->GetComponent<SpriteRenderer>();
				sprite->color.A += Time::GetRealDeltaTime() / GetConfig().BackGround_ChageTime;
			}
			if (lu)
			{
				for (auto& item : *TitleUI->transform->GetChild())
				{
					SpriteRenderer* sprite = item->GetOwner()->GetComponent<SpriteRenderer>();
					sprite->color.A = 1.0f;
				}
			}
		}, true, GetConfig().BackGround_ChageTime, 1);
}



void BackGroundMusic::Awake()
{
	backGroundMsusic = nullptr;
	backGroundMsusic = ResourceManager::GetAudioBank("SFX")->Play("event:/Ambience/Western");
	channel = AudioFramework::Play_BankSound("Sound", "Ambient_Train1", true);

	SetDontDestroyOnSceneChange(true);
}

void BackGroundMusic::OnDestroy()
{
	channel->stop();
	backGroundMsusic->Stop();
	delete backGroundMsusic;
}

void BackGroundMusic::Stop()
{
	channel->setPaused(true);
	backGroundMsusic->Stop();
}

void BackGroundMusic::Play()
{
	channel->setPaused(false);
	backGroundMsusic->Play();
}


void GameSetting::Update()
{
	/*if (Input::IsDown(KeyCode::Esc))
	{
		DG::Core::GetInstance().ShutDownGame();
	}*/
}
