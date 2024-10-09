#pragma once
#include "Engine.h"

class GameManager;

class PrintActionPriority : public Script
{
	Object* textObj;
	SpriteRenderer* renderer;
public:
	void Awake();
	void Start();

	void PrintPriority(int priority);
	void HidePriority();

	~PrintActionPriority();
};

