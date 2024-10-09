#include "PauseMenu.h"
#include "GameManager.h"
#include "ButtonUI.h"
#include "PositionHelper.h"

void PauseMenu::Awake()
{
	SetObjectLayer(ObjectLayer::UI);
	SetDrawingPriority(OrderLayer::pause_menu_background);

	renderer = AddComponent<SpriteRenderer>();
	renderer->nineSlicedImage = ResourceManager::GetNineSlicedImage("UI/Popup");
	renderer->useNineSliced = true;


	btn_skip_tutorial = AddUIObject<HolyButton>();
	btn_skip_tutorial->transform->SetParent(transform);
	btn_skip_tutorial->RegisterSprite("UI/SkipTutorialBtn", "UI/SkipTutorialBtn/Selected");
	btn_skip_tutorial->clicked = [this]()
		{
			this->CloseMenu();
			gameManager->SkipTutorial();
		};

	btn_retry = AddUIObject<HolyButton>();
	btn_retry->transform->SetParent(transform);
	btn_retry->RegisterSprite("UI/RetryBtn", "UI/RetryBtn/Selected");
	btn_retry->clicked = [this]()
		{
			this->CloseMenu();
			gameManager->PrepareRestart();
		};

	btn_resume = AddUIObject<HolyButton>();
	btn_resume->transform->SetParent(transform);
	btn_resume->RegisterSprite("UI/ResumeBtn", "UI/ResumeBtn/Selected");
	btn_resume->clicked = [this]()
		{
			this->CloseMenu();
		};

	btn_go_to_main = AddUIObject<HolyButton>();
	btn_go_to_main->transform->SetParent(transform);
	btn_go_to_main->RegisterSprite("UI/BackToMainBtn", "UI/BackToMainBtn/Selected");
	btn_go_to_main->clicked = [this]()
		{
			this->CloseMenu();
			SceneManager::LoadScene<SampleScene>();
		};

	/*btn_quit = AddUIObject<HolyButton>();
	btn_quit->transform->SetParent(transform);
	btn_quit->RegisterSprite("UI/QuitBtn", "UI/QuitBtn/Selected");
	btn_quit->clicked = [this]()
		{
			DG::Core::GetInstance().ShutDownGame();
		};*/
}

void PauseMenu::Start()
{
}

void PauseMenu::Update()
{
}

void PauseMenu::OpenMenu()
{
	Time::GetInstance().getTimeReturnZero = true;
	UpdateMenuShapeAndContent();
	SetEnable(true);
}

void PauseMenu::CloseMenu()
{
	Time::GetInstance().getTimeReturnZero = false;
	SetEnable(false);
}

void PauseMenu::UpdateMenuShapeAndContent()
{
	if (gameManager->GetCurrentStage() <= 2)
	{
		renderer->SetSize(600, 620);
		btn_skip_tutorial->SetEnable(true);

		// ¿©¹é 40 (600 = 400 + 5x)
		btn_resume->transform->SetPositionY(btn_gap_y * 1.5f);
		btn_skip_tutorial->transform->SetPositionY(btn_gap_y * 0.5f);
		btn_retry->transform->SetPositionY(-btn_gap_y * 0.5f);
		btn_go_to_main->transform->SetPositionY(-btn_gap_y * 1.5f);
		//btn_quit->transform->SetPositionY(-210);

		if (gameManager->GetCurrentPhase() != Phase::Type::Player_Action)
		{
			btn_skip_tutorial->TurnOffButton();
		}
		else
		{
			btn_skip_tutorial->TurnOnButton();
		}
	}
	else
	{
		renderer->SetSize(600, 480);
		btn_skip_tutorial->SetEnable(false);

		btn_resume->transform->SetPositionY(btn_gap_y);
		btn_retry->transform->SetPositionY(0);
		btn_go_to_main->transform->SetPositionY(-btn_gap_y);
		//btn_quit->transform->SetPositionY(-140);
	}
}