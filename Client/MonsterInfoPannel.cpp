#include "MonsterInfoPannel.h"
#include "WaveObject.h"

MonsterInfoPannel::~MonsterInfoPannel()
{

}

void MonsterInfoPannel::Awake()
{
	transform->SetPosition(-20.f, -20.f, 0);


}

void MonsterInfoPannel::Update()
{
 	int count = waveInfo.size();

	if (count == 2) {
		waveInfo[0]->transform->SetPositionX(-80.f);
		waveInfo[1]->transform->SetPositionX(80.f);
	}
	else if (count == 3) {
		waveInfo[0]->transform->SetPositionX(-140.f);
		waveInfo[2]->transform->SetPositionX(140.f);
	}
}

void MonsterInfoPannel::AddWaveObjectInfo(int objectId, int spawnCount)
{
	WaveObject* waveObject = AddUIObject<WaveObject>();
	waveObject->transform->SetParent(transform);
	waveObject->SetSpawnCount(spawnCount);
	waveObject->SetDrawingPriority(0);
	
	switch (objectId) {
	case 1:
		waveObject->SetMonsterImage("rushEnemy/idle");
		break;
	case 2:
		waveObject->SetMonsterImage("meleeEnemy/idle");
		break;
	case 3:
		waveObject->SetMonsterImage("rangeEnemy/idle");
		break;
	default:
		waveObject->SetMonsterImage("meleeEnemy/idle");
		break;
	}

	waveInfo.push_back(waveObject);
}

void MonsterInfoPannel::ObjectInfoClear()
{
	for (int i = 0; i < waveInfo.size(); i++) {
		waveInfo[i]->Kill();
	}
	waveInfo.clear();
}
