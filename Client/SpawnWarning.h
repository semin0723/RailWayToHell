#pragma once
#include "Engine.h"

class SpawnWarning : public Object
{
public:
	SpawnWarning() {}
	~SpawnWarning() {}

	virtual void Awake() override;
	
	void SetMonsterKey(std::string key);

private:
	SpriteRenderer* spriteRenderer;

	std::string key = "UI/Warning";
};

