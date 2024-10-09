#pragma once
#include "Engine.h"
#include "StageInfo.h"
#include "MonsterInfoPannel.h"
#include "TextObject.h"

class NextWaveInfoPannel : public Object
{
public:
	NextWaveInfoPannel() {}
	~NextWaveInfoPannel() {}

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	void TurnStart(IStageInfo* stageInfo, int curStage);
	void TurnEnd();
private:
	SpriteRenderer* spriteRenderer;

	TextObject* qm = nullptr;

	MonsterInfoPannel* monsterSpawnInfoPannel;
};

