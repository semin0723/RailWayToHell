#include "TurnPannel.h"
#include "GameManager.h"
#include "TurnObject.h"
#include "WaveIcon.h"
#include "Engine.h"

void TurnPannel::Awake()
{
	transform->SetPosition(Vector3(0.0f, 400.f, 0));
}

void TurnPannel::Start()
{
}

void TurnPannel::Update()
{
}

void TurnPannel::OnDestroy()
{
}

void TurnPannel::StageEnd()
{
	for (auto& candle : childs) {
		candle->Kill();
	}
	childs.clear();
}

void TurnPannel::StageStart(IStageInfo* info)
{
	int max_turn = info->GetMaxTurn();
	float init_x = 0;
	float gap_x = 60.0f;
	if (max_turn % 2 == 1)
	{
		init_x = -gap_x * (max_turn / 2);
	}
	else if (max_turn % 2 == 0)
	{
 		init_x = (gap_x * 0.5f) - (gap_x * (max_turn * 0.5f));
	}

	float cur_x = init_x;
	for (int i = 0; i < max_turn; ++i)
	{
		Object* candle = AddUIObject<TurnObject>();
		candle->transform->SetParent(transform);
		candle->transform->SetPositionX(cur_x);
		childs.push_back(candle);
		cur_x += gap_x;
	}
	for (int i = 2; i <= max_turn; ++i)
	{
		if (info->IsWaveTurn(i) == true)
		{
			Object* waveTurn = AddUIObject<WaveIcon>();
			waveTurn->transform->SetPosition(Vector3(-gap_x * 0.416f, 75.f, 0));
			waveTurn->transform->SetParent(childs[i - 1]->transform);
		}
	}
}

void TurnPannel::TurnEnd(int curTurn)
{
	if (curTurn < 1 || curTurn >= childs.size()) return;
   	childs[curTurn - 1]->GetComponent<SpriteRenderer>()->SetImage(ResourceManager::GetImage("UI/CandleCut"));
}
