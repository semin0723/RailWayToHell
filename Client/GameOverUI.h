#pragma once
#include "Engine.h"

class GameOverUI : public Object
{
	Object* blackBack;
	Object* gameOverText;

	class Player* player;

	//class ImageUI* restartUI;
	//class ImageUI* mainMenuUI;
	class HolyButton* restartButton;
	class HolyButton* mainMenuButton;

	float fade_in_duration = 1.0f;
	float player_init_move_time = 0.3f;
	float player_move_duration = 0.7f;

	FMOD::Channel* gameOverSound;
public:
	void Awake() override;
	void OnDestroy() override;
	void Start() override;
};

