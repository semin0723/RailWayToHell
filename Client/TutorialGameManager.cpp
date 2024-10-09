#include "TutorialGameManager.h"
#include "TurnLeft.h"
#include "TacticModeBtn.h"

void TutorialGameManager::Awake()
{
	gameManager = this;

	//========================
	//     스테이지 셋팅
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
	//        UI 셋팅
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

	// TODO: stepInProgress의 스위치는 각 스탭에 맞는 행동을 하면 false로 바꿔주고, 스탭을 증가시킵니다.
	// TODO: 각 스탭의 Initialize를 여기서 수행합니다.

	if (tutorialStep == 1) {
		// TODO: 여기서 좌 우 이동 키를 알려주는 UI를 생성하고, 몬스터를 스폰합니다. 초기 설정 완료 시점.
	}


	stepInProgress = true;
}

void TutorialGameManager::SpawnMonster(int curStage, int curTurn)
{

}
