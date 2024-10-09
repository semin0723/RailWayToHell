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
		S1T1Start,	// UI���
		S1T1Aim,	// UI���
		S1T2Start,	// 1�� ���� ���� UI���
		S1T2Aim,	// UI���
		S1T3Start,	// 1�� ���� ���� UI���
		S1T3Aim_1,	// ���� ���� UI���
		S1T3Aim_2,	// 1�� ���� ���� UI���
		S1End,		// UI���

		S2T1Start_1,// 1�� ���� ����
		S2T1Start_2,// UI ���, ��Ƽ�� ��ư�� ���콺 �÷�����
		S2T1Start_2_ING,// UI ���, ��Ƽ�� ��ư�� ���콺 �÷�����
		S2T1Start_3,// 1�� ���?
		S2T1Start_4,// A������ �̵�
		S2T1Aim,	// UI���, ȭ���� Ŭ���ؾ���
		S2T2Start,	// D������ �̵�, UI���
		S2T2Aim,	// UI���, ���鸮�� �����ؾ���
		S2T3Start,	// 1�� ���� ���� UI��� A��������
		S2T3Aim,	// UI���, ���鸮�� �����ؾ���
		S2T4Start,	// 1�� ���� ���� UI���
		S2T4Aim,

		TutorialEnd,//Ʃ�丮�� ��

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

