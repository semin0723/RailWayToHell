#pragma once
#include "Engine.h"
#include "TurnPannel.h"
#include "NextWaveInfoPannel.h"
#include "Cursor_Aim.h"
#include "Highlightable.h"

/*
	�� ���� �Ŵ����� Ʃ�丮���� ���� �Ŵ��� �Դϴ�.
	������ �������� �����ϱ� ���� ������� ���� �Ŵ��� �Դϴ�.

	***<   �ڵ� ���� ���� �ȹ���   >***
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
	// Ʃ�丮���� ���൵�� ��Ÿ��, �� ���൵�� ���� ����� ���������� �߰��� ����.
	int curTurn			= 1;
	int curStage		= 1;
	int tutorialStep	= 1;

	// stepInProgress == true	-> ���� �ش� ���� ������
	// stepInProgress == false	-> ���� �������� �Ѿ �� �ִ�.
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

