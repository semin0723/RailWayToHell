#pragma once
#include "Engine.h"

class WaveObject;

class MonsterInfoPannel : public Object
{
public:
	MonsterInfoPannel() {}
	~MonsterInfoPannel();

	virtual void Awake() override;
	virtual void Update() override;
	

	void AddWaveObjectInfo(int objectId, int spawnCount);

	void ObjectInfoClear();
private:
	std::vector<WaveObject*> waveInfo;
};

