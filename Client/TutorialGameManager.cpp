#include "TutorialGameManager.h"
#include "TurnLeft.h"
#include "TacticModeBtn.h"

void TutorialGameManager::Awake()
{
	gameManager = this;

	//========================
	//     �������� ����
	//========================
	tutoStage1SpawnInfo = {
		{ 2, -1, -1, -1, -1, -1,  2},
		{-1, -1, -1,  2, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1}
	};

	tutoStage2SpawnInfo = {
		{ 2, -1,  1, -1,  3,  3,  2},
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1,  2, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1}
	};

	tileInfo.resize(7);

	//========================
	//        UI ����
	//========================
	turnUI			= AddUIObject<TurnPannel>();
	waveInfoUI		= AddUIObject<NextWaveInfoPannel>();
	AddObject<TurnLeft>();
	//AddUIObject<TacticModeBtn>("TacticModeBtn");
	//cursor_aim = GetOwner()->GetObjectByName<Cursor_Aim>("Cursor");
}

void TutorialGameManager::Start()
{

}

void TutorialGameManager::Update()
{
	if (stepInProgress == true) return;

	// TODO: stepInProgress�� ����ġ�� �� ���ǿ� �´� �ൿ�� �ϸ� false�� �ٲ��ְ�, ������ ������ŵ�ϴ�.
	// TODO: �� ������ Initialize�� ���⼭ �����մϴ�.

	if (tutorialStep == 1) {
		// TODO: ���⼭ �� �� �̵� Ű�� �˷��ִ� UI�� �����ϰ�, ���͸� �����մϴ�. �ʱ� ���� �Ϸ� ����.
	}


	stepInProgress = true;
}

void TutorialGameManager::SpawnMonster(int curStage, int curTurn)
{

}
