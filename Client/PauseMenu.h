#pragma once
#include "Engine.h"

class HolyButton;

class PauseMenu : public Object
{
	SpriteRenderer* renderer;

	HolyButton* btn_skip_tutorial;
	HolyButton* btn_retry;
	HolyButton* btn_resume;
	HolyButton* btn_go_to_main;
	//HolyButton* btn_quit;

	float btn_gap_y = 140;
public:
	class GameManager* gameManager;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OpenMenu();
	void CloseMenu();

	void UpdateMenuShapeAndContent();
};

