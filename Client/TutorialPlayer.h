#pragma once
#include "Engine.h"
#include "GameObject.h"

class TutorialPlayer : public GameObject
{
public:
	TutorialPlayer() {}
	~TutorialPlayer() {}

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	
	// 본 플레이어는 하드코딩으로 전부 채울 예정. 오로지 튜토리얼 전용
	/*
		필요한 요소 :
		이동, 공격, 무기 변경
	*/

private:

};

