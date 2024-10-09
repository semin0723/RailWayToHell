#pragma once
#include "Engine.h"
#include "StageInfo.h"

class TurnPannel : public Object
{
public:
	TurnPannel() {}
	~TurnPannel() {}

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	void StageEnd		();
	void StageStart		(IStageInfo* info);
	void TurnEnd		(int curTurn);
private:
	std::vector<Object*> childs;

	SpriteRenderer* spriteRenderer;
};

