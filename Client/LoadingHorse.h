#pragma once
#include "Engine.h"

class TitleScene;

class LoadingHorse : public Object
{
	SpriteRenderer* renderer;
	Animator* animator;
public:
	TitleScene* scene;
	void Awake() override;
	void OnDestroy() override;
	void Update() override;

	float elapsedtime = 1.0f;
};

