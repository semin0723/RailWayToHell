#include "NextWaveInfoPannel.h"
#include "NextTurnText.h"
#include "WaveIcon.h"
#include "WaveObject.h"
#include "MonsterInfoPannel.h"
#include "GameManager.h"
#include <format>

void NextWaveInfoPannel::Awake()
{
	transform->SetPosition(650.f, 400.0f, 0);

	spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetImage(ResourceManager::GetImage("UI/BackUI"));
	spriteRenderer->SetSize(550, 200);

	monsterSpawnInfoPannel = AddUIObject<MonsterInfoPannel>();
	monsterSpawnInfoPannel->transform->SetParent(transform);

	Object* text = AddUIObject<NextTurnText>();
	text->transform->SetPosition(-100.f, 80.f, 0);
	text->transform->SetParent(transform);
	

	Object* waveIcon = AddUIObject<WaveIcon>();
	waveIcon->transform->SetPosition(-125.f, 50.f, 0);
	waveIcon->transform->SetParent(transform);

	qm = AddUIObject<TextObject>();
	qm->transform->SetPosition(-40.f, 20.f, 0);
	qm->transform->SetParent(transform);


}

void NextWaveInfoPannel::Start()
{
}

void NextWaveInfoPannel::Update()
{
	if (GetOwner()->GetObjectByName<GameManager>("GameManager")->GetCurrentStage() == 6 || GetOwner()->GetObjectByName<GameManager>("GameManager")->GetCurrentStage() == 7) {
		qm->GetComponent<TextRenderer>()->fontSize = 50.f;
		qm->GetComponent<TextRenderer>()->text = L"????";
	}
	else {
		qm->GetComponent<TextRenderer>()->text = L"";
	}

}

void NextWaveInfoPannel::TurnStart(IStageInfo* stageInfo, int nextStage)
{
	if (nextStage > stageInfo->GetMaxTurn() && stageInfo->GetMaxTurn() != -1) return;

	std::vector<int> spawnPos, spawnWeight;

	stageInfo->GetSpawnPos(spawnPos, spawnWeight, nextStage);
	int rush = 0;
	int melee = 0;
	int range = 0;

	for (int i = 0; i < spawnPos.size(); i++) {
		switch (spawnPos[i]) {
		case 1:
			rush++;
			break;
		case 2:
			melee++;
			break;
		case 3:
			range++;
			break;
		}
	}
	switch (stageInfo->GetTrackingSpawn(nextStage))
	{
	case 1:
		rush++;
		break;
	case 2:
		melee++;
		break;
	case 3:
		range++;
		break;
	}
	if (spawnPos.size() || stageInfo->GetTrackingSpawn(nextStage) != -1)
	{
		if (rush)
		{
			monsterSpawnInfoPannel->AddWaveObjectInfo(1, rush);
		}
		if (melee)
		{
			monsterSpawnInfoPannel->AddWaveObjectInfo(2, melee);
		}
		if (range)
		{
			monsterSpawnInfoPannel->AddWaveObjectInfo(3, range);
		}
	}
	std::cout << std::format("rush {}, melee {}, range {}\n", rush, melee, range);
}

void NextWaveInfoPannel::TurnEnd()
{
	monsterSpawnInfoPannel->ObjectInfoClear();
}
