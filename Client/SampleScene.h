#pragma once
#include "Engine.h"

class SampleScene : public Scene
{
public:
	Object* TitleUI;
	SpriteRenderer* title_logo_renderer;

	~SampleScene();

public:
	class GameManager* gameManager;
	void Initialize() override;
};
class BackGroundMusic : public Object
{
public:
	void Awake() override;
	void OnDestroy() override;

public:
	void Stop();
	void Play();

public:
	FMOD::Channel* channel;
	class SoundBankChannel* backGroundMsusic;
};

