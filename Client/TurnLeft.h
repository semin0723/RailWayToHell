#pragma once
#include "Engine.h"

class TurnLeft : public Object
{
	int turn = 5;
	TextRenderer* renderer;
public:
	//void Awake();

	//void Update();

	class GameManager* gameManager;
};