#include "StageGoalPannel.h"
#include "GameManager.h"

void StageGoalPannel::Awake()
{
	spriteRenderer = AddComponent<SpriteRenderer>();

	spriteRenderer->SetImage(ResourceManager::GetImage("UI/BackUI"));
	spriteRenderer->SetSize(550, 200);
	transform->SetScaleX(-1);

	transform->SetPosition(-650.f, 400.f, 0);

	turnLeft = AddUIObject<TextObject>();
	turnLeft->transform->SetPosition(150.f, 65.f, 0);
	turnLeft->transform->SetScaleX(-1);
	turnLeft->transform->SetParent(transform);

	turnLeft->GetComponent<TextRenderer>()->text = L"0";
	turnLeft->GetComponent<TextRenderer>()->fontSize = 80.f;

	explainGoal = AddUIObject<TextObject>();
	explainGoal->transform->SetPosition(80.f, 50.f, 0);
	explainGoal->transform->SetScaleX(-1);
	explainGoal->transform->SetParent(transform);

	//explainGoal->GetComponent<TextRenderer>()->SetAlignment(TextAlignHorizontal::Center);
	explainGoal->GetComponent<TextRenderer>()->text = L"턴 내 모든 적 처치.";

	goalInfo = AddUIObject<TextObject>();
	goalInfo->transform->SetPosition(30.f, 0, 0);
	goalInfo->transform->SetScaleX(-1);
	goalInfo->transform->SetParent(transform);

	//goalInfo->GetComponent<TextRenderer>()->SetAlignment(TextAlignHorizontal::Center);
}

void StageGoalPannel::Start()
{
	gameManager = GetOwner()->GetObjectByName<GameManager>("GameManager");
}

void StageGoalPannel::Update()
{
	if (currentTurn < 0) currentTurn = 0;
}

void StageGoalPannel::SetTotalSpawnMonster(int count)
{
	totalSpawnMonster = count;
	UpdateStageGoalPanel();
}

void StageGoalPannel::SetCurrentKillMonster(int count)
{
	currentSpawnMonster = count;
	UpdateStageGoalPanel();
}

void StageGoalPannel::SetCurrentTurn(int turn)
{
	currentTurn = turn;
	UpdateStageGoalPanel();
}

void StageGoalPannel::UpdateStageGoalPanel()
{
	if (gameManager == nullptr) return;
	int stageNum = gameManager->GetCurrentStage();

	if (currentTurn < 0) currentTurn = 0;

	switch (stageNum)
	{
	// 1 - 2 튜토리얼
	case 1:
	case 2:
		{
			turnLeft->GetComponent<TextRenderer>()->text = L"∞";
			turnLeft->transform->SetPosition(170.f, 60.f, 0);
			explainGoal->GetComponent<TextRenderer>()->text = L"턴 내 모든 적 처치.";
			explainGoal->GetComponent<TextRenderer>()->fontSize = 40.f;
			goalInfo->GetComponent<TextRenderer>()->text = L"";
			explainGoal->transform->SetPosition(100.f, 25.f, 0);
		}
		break;
	// 3 - 5 일반
	case 3:
	case 4:
	case 5:
		{
			turnLeft->GetComponent<TextRenderer>()->text = std::to_wstring(currentTurn);
			goalInfo->GetComponent<TextRenderer>()->text = std::to_wstring(currentSpawnMonster) + L" / " + std::to_wstring(totalSpawnMonster);
			explainGoal->transform->SetPosition(80.f, 50.f, 0);
		}
		break;
	// 6 - 7 보스, 심장
	case 6:
		{
			turnLeft->GetComponent<TextRenderer>()->text = L"";
			explainGoal->GetComponent<TextRenderer>()->text = L"군주 매피스토 처치";
			explainGoal->GetComponent<TextRenderer>()->fontSize = 40.f;
			explainGoal->transform->SetPosition(140.f, 25.f, 0);
			goalInfo->GetComponent<TextRenderer>()->text = L"";
		}
		break;
	case 7:
		{
			turnLeft->GetComponent<TextRenderer>()->text = L"";
			explainGoal->GetComponent<TextRenderer>()->text = L"열차의 심장 처치.";
			explainGoal->GetComponent<TextRenderer>()->fontSize = 40.f;
			explainGoal->transform->SetPosition(140.f, 25.f, 0);
			goalInfo->GetComponent<TextRenderer>()->text = L"";
		}
		break;
	default:
		break;
	}
}
