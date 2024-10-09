#pragma once
#include "Engine.h"



class TitleScene : public Scene
{
public:
	
	void Initialize() override;
	void LoadResource();

	Object* intro;


};

