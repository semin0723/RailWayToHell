#include "CreditScene.h"
#include "TrainSpriteObject.h"
#include "StageInfo.h"
#include "SampleScene.h"
#include "Player.h"
#include "TrashAnimation.h"

class CreditObject : public Object
{
	void Awake() override
	{
		std::vector<TrainInfo> tempTrainInfoInfos;
		DataAsset::LoadDataAsset<TrainInfo>("Resource/StageInfo/CreditTrainData.csv", tempTrainInfoInfos);

		for (auto& item : tempTrainInfoInfos)
		{
			trainInfo.emplace(item.Stage, std::move(item));
		}
		currentTrainIndex = (--trainInfo.end())->first;

		for (auto& item : GetOwner()->GetObjectsByTag("TrainSpriteObject"))
		{
			TrainSpriteObject* trainSprite = dynamic_cast<TrainSpriteObject*>(item);
			trainSprite->SetDontDestroyOnSceneChange(false);
			trainSprite->SetEnable(true);
			trainSprite->AddRealTimeTimerFunction([=](bool lu)
												  {
													  if (lu)
													  {
														  trainSprite->Kill();
													  }
												  }, true, 0.5f, 1);


		}


		maintrain = AddObject<TrainSpriteObject>();
		TrainInfo& currentTrainInfo = trainInfo[currentTrainIndex];
		maintrain->Init(currentTrainInfo.TrainName, currentTrainInfo.BackGround);
		maintrain->SetIsCredit(true);
		maintrain->SetReverse(true);
		maintrain->SetIsWallDisappear(false);
		maintrain->Default();
		maintrain->train->SetDrawingPriority(OrderLayer::Train);

		oldtrain = AddObject<TrainSpriteObject>();
		currentTrainInfo = trainInfo[--currentTrainIndex];
		oldtrain->Init(currentTrainInfo.TrainName, currentTrainInfo.BackGround);
		oldtrain->SetIsCredit(true);
		oldtrain->SetReverse(true);
		oldtrain->SetIsWallDisappear(false);
		oldtrain->OutOut();
		oldtrain->train->SetDrawingPriority(OrderLayer::SubTrain);

		Player* player = GetOwner()->GetObjectByName<Player>("Player");
		if (player)
		{
			player->GetComponent<HealthComponent>()->enable = false;
			player->SetDontDestroyOnSceneChange(false);
			player->FadeOut(1.0f);
		}

		Object* Lever = GetOwner()->GetObjectByName<Object>("Lever");
		if (Lever)
		{
			Lever->SetDontDestroyOnSceneChange(false);
			AddRealTimeTimerFunction([Lever](bool lu)
									 {
										 Lever->GetComponent<SpriteRenderer>()->color.A -= Time::GetDeltaTime();
										 if (lu)
										 {
											 Lever->Kill();
										 }
									 }, true, 1.0f, 1);

		}
		elapsedTime = 5.0f;
	}

	void Update() override
	{
		elapsedTime += Time::GetDeltaTime();
		if (elapsedTime >= 7.0f)
		{
			elapsedTime = 0.0f;
			if (!maintrain)
			{
				GetOwner()->GetObjectByName<BackGroundMusic>("BackGroundMusic")->SetDontDestroyOnSceneChange(false);
				SceneManager::LoadScene<SampleScene>();
			}
			NextCredit();
		}
		if (Input::IsDown(KeyCode::Esc))
		{
			GetOwner()->GetObjectByName<BackGroundMusic>("BackGroundMusic")->SetDontDestroyOnSceneChange(false);
			SceneManager::LoadScene<SampleScene>();
		}


	}

	void NextCredit()
	{
		if (maintrain)
		{
			maintrain->Come();
			maintrain->train->SetDrawingPriority(OrderLayer::SubTrain);
		}
		if (oldtrain)
		{
			auto tempTrain = oldtrain;
			oldtrain->Out();
			oldtrain->train->SetDrawingPriority(OrderLayer::Train);

			AddTimerFunction([tempTrain](bool lastUpdate)
							 {
								 tempTrain->Default();
							 }, false, 5.0f, 1);
		}



		auto tempTrain = maintrain;
		AddTimerFunction([this, tempTrain](bool lastUpdate)
						 {
							 tempTrain->ComeCome();
						 }, false, isFinalCredit ? 4.3f : 7.0f, 1);

		AddTimerFunction([this, tempTrain](bool lastUpdate)
						 {
							 tempTrain->Kill();
						 }, false, 8.0f, 1);

		if (isFinalCredit)
		{
			maintrain = nullptr;
			return;
		}


		maintrain = oldtrain;
		oldtrain = nullptr;

		TrainInfo& currentTrainInfo = trainInfo[--currentTrainIndex];
		if (currentTrainIndex == 0)
		{
			isFinalCredit = true;
			return;
		}
		oldtrain = AddObject<TrainSpriteObject>();
		oldtrain->Init(currentTrainInfo.TrainName, currentTrainInfo.BackGround);
		oldtrain->SetIsCredit(true);
		oldtrain->SetReverse(true);
		oldtrain->SetIsWallDisappear(false);
		oldtrain->train->SetDrawingPriority(OrderLayer::SubTrain);

		AddTimerFunction([this](bool lastUpdate)
						 {
							 oldtrain->OutOut();
						 }, false, 5.0f - 1.4f, 1);
		
	}

	std::map<int, TrainInfo> trainInfo;
	int currentTrainIndex;
	TrainSpriteObject* maintrain;
	TrainSpriteObject* oldtrain;

	bool isFinalCredit;
	float elapsedTime;
};

void CreditScene::Initialize()
{
	AddObject<CreditObject>();
}
