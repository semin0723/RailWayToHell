#pragma once
#include "Engine.h"

class WaveObject : public Object
{
public:
	WaveObject() {}
	~WaveObject() {}

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	void SetSpawnCount(int count);
	void SetMonsterImage(std::string key);

private:
	Object* spawnText;
	SpriteRenderer* spriteRenderer;
	int spawnCount = 0;
};

