#pragma once
#include "Engine.h"
#include "TextObject.h"

class StageGoalPannel : public Object
{
	class GameManager* gameManager{ nullptr };
public:
	StageGoalPannel() {}
	~StageGoalPannel() {}

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	void SetTotalSpawnMonster(int count);
	void SetCurrentKillMonster(int count);
	void SetCurrentTurn(int turn);
	/** 좌상단 UI 패널의 문구를 교체합니다 */
	void UpdateStageGoalPanel();
private:
	int totalSpawnMonster = 0;
	int currentSpawnMonster = 0;

	int currentTurn = 0;

	SpriteRenderer*	spriteRenderer		= nullptr;

	TextObject*		turnLeft			= nullptr;
	TextObject*		explainGoal			= nullptr;
	TextObject*		goalInfo			= nullptr;
};

