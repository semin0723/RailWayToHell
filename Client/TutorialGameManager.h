#pragma once
#include "Engine.h"
#include "TurnPannel.h"
#include "NextWaveInfoPannel.h"
#include "Cursor_Aim.h"
#include "Highlightable.h"

/*
	본 게임 매니저는 튜토리얼만을 위한 매니저 입니다.
	게임의 정보만을 전달하기 위해 만들어진 게임 매니저 입니다.

	***<   코드 구조 지적 안받음   >***
*/

class GameObject;
class TileObject;

class TutorialGameManager : public Object
{
public:
	using TileInfo		= std::vector<TileObject*>;
	using SpawnInfo		= std::vector<std::vector<int>>;

	TutorialGameManager() {}
	~TutorialGameManager() {}

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	const int GetCurStage()				const	{ return curStage; }
	const int GetCurTutorialStep()		const	{ return tutorialStep; }

	TutorialGameManager* gameManager	= nullptr;
private:
	// 튜토리얼의 진행도를 나타냄, 이 진행도에 따라 기능이 순차적으로 추가될 예정.
	int curTurn			= 1;
	int curStage		= 1;
	int tutorialStep	= 1;

	// stepInProgress == true	-> 아직 해당 스탭 진행중
	// stepInProgress == false	-> 다음 스탭으로 넘어갈 수 있다.
	bool stepInProgress = true;
	

	TileInfo tileInfo;

	SpawnInfo tutoStage1SpawnInfo;
	SpawnInfo tutoStage2SpawnInfo;


	// UI Place
	TurnPannel*				turnUI				= nullptr;
	NextWaveInfoPannel*		waveInfoUI			= nullptr;

	// ETC
	Cursor_Aim*				cursor_aim			= nullptr;
	Object*					shade				= nullptr;
	HighlightManager*		highlightManager	= nullptr;


	void SpawnMonster(int curStage, int curTurn);
};

