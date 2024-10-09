#pragma once
#include "Engine.h"


class PlayerState : public BT::BehaviourTreeV2
{
#define PlayerStateReflect(x)
	GENERATED_BODY(PlayerState, BehaviourTreeV2)

public:
	void Init();
};

namespace ETutorialPhase
{
	enum Type
	{
		S1T1Start,	// UI출력
		S1T1Aim,	// UI출력
		S1T2Start,	// 1초 조작 막고 UI출력
		S1T2Aim,	// UI출력
		S1T3Start,	// 1초 조작 막고 UI출력
		S1T3Aim_1,	// 조작 막고 UI출력
		S1T3Aim_2,	// 1초 조작 막고 UI출력
		S1End,		// UI출력

		S2T1Start_1,// 1초 조작 막고
		S2T1Start_2,// UI 출력, 택티컬 버튼에 마우스 올려야함
		S2T1Start_2_ING,// UI 출력, 택티컬 버튼에 마우스 올려야함
		S2T1Start_3,// 1초 대기?
		S2T1Start_4,// A눌러야 이동
		S2T1Aim,	// UI출력, 화약통 클릭해야함
		S2T2Start,	// D눌러야 이동, UI출력
		S2T2Aim,	// UI출력, 샹들리에 선택해야함
		S2T3Start,	// 1초 조작 막고 UI출력 A눌러야함
		S2T3Aim,	// UI출력, 샹들리에 선택해야함
		S2T4Start,	// 1초 조작 막고 UI출력
		S2T4Aim,

		TutorialEnd,//튜토리얼 끝

		MAX
	};

}
class PlayeTutorialrState : public BT::BehaviourTreeV2
{
#define PlayeTutorialrStateReflect(x)
	GENERATED_BODY(PlayeTutorialrState, BehaviourTreeV2)
public:
	void Init();

	ETutorialPhase::Type GetTutorialState() { return tutorialPhase; }
	void PrevTutorialState()
	{
		tutorialPhase = ETutorialPhase::Type(tutorialPhase - 1);
		if (tutorialPhase < ETutorialPhase::S1T1Start)
		{
			tutorialPhase = ETutorialPhase::S1T1Start;
		}
	}
	
	void NextTutorialState();

	ETutorialPhase::Type tutorialPhase;
};

