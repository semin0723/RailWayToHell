#pragma once
#include "Engine.h"
#include "Config.h"

// Retry시 씬 재로드용

class GameState : public Object
{
public:
	int current_stage_number{ 1 };
	bool isRestart{ false };

	void Awake() override
	{
		SetDontDestroyOnSceneChange(true);
	}
	void Start() override
	{
		current_stage_number = GetConfig().GameManager_StartStageNumber;
	}
	void Update() override
	{

	}
};