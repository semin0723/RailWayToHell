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
	
	// �� �÷��̾�� �ϵ��ڵ����� ���� ä�� ����. ������ Ʃ�丮�� ����
	/*
		�ʿ��� ��� :
		�̵�, ����, ���� ����
	*/

private:

};

