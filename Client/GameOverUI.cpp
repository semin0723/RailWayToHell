#include "GameOverUI.h"
#include "ImageUI.h"
#include "ButtonUI.h"
#include "SampleScene.h"
#include "GameManager.h"
#include "Player.h"

void GameOverUI::Awake()
{
	gameOverSound = AudioFramework::Play_BankSound("Sound", "BGM_GameOver");
	blackBack = AddUIObject<Object>();
	blackBack->SetDrawingPriority(OrderLayer::GameOver);
	blackBack->AddComponent<SpriteRenderer>()->SetImage("ggamjange");
	blackBack->GetComponent<SpriteRenderer>()->SetSize(2200, 1200);

	gameOverText = AddUIObject<Object>();
	gameOverText->SetDrawingPriority(OrderLayer::GameOver_ui);
	gameOverText->AddComponent<SpriteRenderer>()->SetImage("UI/GameOverText");
	gameOverText->GetComponent<SpriteRenderer>()->SetSizeAuto();
	gameOverText->transform->SetPosition(Camera::GetScreenPointByRatio(0.5f, 0.75f));

	

	restartButton = AddUIObject<HolyButton>();
	restartButton->RegisterSprite("UI/RetryBtn", "UI/RetryBtn/Selected");
	restartButton->clicked = [this]()
		{
			Kill();
			GetOwner()->GetObjectByName<GameManager>("GameManager")->PrepareRestart();
		};
	restartButton->transform->SetPosition(Camera::GetScreenPointByRatio(0.5f, 0.27f));

	mainMenuButton = AddUIObject<HolyButton>();
	mainMenuButton->RegisterSprite("UI/BackToMainBtn", "UI/BackToMainBtn/Selected");
	mainMenuButton->clicked = [this]()
		{
			Kill();
			SceneManager::LoadScene<SampleScene>();
		};
	mainMenuButton->transform->SetPosition(Camera::GetScreenPointByRatio(0.5f, 0.13f));



	restartButton->SetDrawingPriority(OrderLayer::GameOver_ui);
	mainMenuButton->SetDrawingPriority(OrderLayer::GameOver_ui);

	blackBack->transform->SetParent(transform);
	gameOverText->transform->SetParent(transform);
	restartButton->transform->SetParent(transform);
	mainMenuButton->transform->SetParent(transform);

	for (auto child : *transform->GetChild())
	{
		child->GetOwner()->GetComponent<SpriteRenderer>()->color.A = 0.0f;
	}
}

void GameOverUI::OnDestroy()
{
	if (gameOverSound)
	{
		gameOverSound->stop();
	}
}

void GameOverUI::Start()
{
	player = GetOwner()->GetObjectByName<Player>("Player");

	Vector3 player_init_pos = player->transform->GetPosition();
	float t = 0.0f;
	AddRealTimeTimerFunction([=](bool lu) mutable
		{
			t += Time::GetRealDeltaTime() / fade_in_duration;


			for (auto child : *transform->GetChild())
			{
				child->GetOwner()->GetComponent<SpriteRenderer>()->color.A = Math::EaseInCubic(0.0f, 1.0f, t);
			}

			if (lu)
			{
				for (auto child : *transform->GetChild())
				{
					child->GetOwner()->GetComponent<SpriteRenderer>()->color.A = 1.0f;
				}
			}
		}, true, fade_in_duration, 1);
	AddRealTimeTimerFunction([=](bool lu) mutable
		{
			AddRealTimeTimerFunction([=](bool lu) mutable
				{
					t += Time::GetRealDeltaTime() / player_move_duration;

					player->transform->SetPosition(Math::EaseInCubic(player_init_pos, Vector3{ 0, -100, 0 }, t));
				}, true, player_move_duration, 1);
		}, false, player_init_move_time, 1);
}
