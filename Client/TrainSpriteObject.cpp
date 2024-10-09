#include "TrainSpriteObject.h"
#include "TrashAnimation.h"

#include <string>
#include <filesystem>
using namespace std::string_literals;

void TrainSpriteObject::Awake()
{
	AddTag("TrainSpriteObject");
	isWallDisappear = true;
}

void TrainSpriteObject::Init(std::string trainName, std::string backGroundName)
{
	if (trainName.empty()) trainName = "main";
	this->trainName = trainName;
	wtrainName.assign(trainName.begin(), trainName.end());
	this->backGroundName = backGroundName;
	wbackGroundName.assign(backGroundName.begin(), backGroundName.end());
	trainState = ETrainState::Init;

	light = AddObject<TrashAnimation>();
	light->SetDrawingPriority(OrderLayer::Light);
	light->transform->SetParent(transform);

	light2 = AddObject<TrashAnimation>();
	light2->SetDrawingPriority(OrderLayer::Light);
	light2->transform->SetParent(transform);

	trainWheel = AddObject<TrashAnimation>();
	trainWheel->SetDrawingPriority(OrderLayer::TrainWheel);
	trainWheel->transform->SetParent(transform);

	train = AddObject<TrashAnimation>();
	train->SetDrawingPriority(OrderLayer::Train);
	train->transform->SetParent(transform);

	trainInside = AddObject<TrashAnimation>();
	trainInside->SetDrawingPriority(OrderLayer::TrainInside);
	trainInside->transform->SetParent(transform);

	trainWall = AddObject<TrashAnimation>();
	trainWall->SetDrawingPriority(OrderLayer::TrainOutSide);
	trainWall->transform->SetParent(transform);
}

float temp = 1.4f;

void TrainSpriteObject::ComeCome()
{
	trainState = ETrainState::ComeCome;
	if (train)
	{
		train->OnDestroy();
		if (std::filesystem::path path(L"Resource/Stage/train_"s + wtrainName + L"_come_come/"s);
			std::filesystem::exists(path))
		{
			train->path = L"Resource/Stage/train_"s + wtrainName + L"_come_come/"s;
			train->key = "train_"s + trainName + "_come_come";
		}
		else
		{
			train->path = L"Resource/Stage/train_main_come_come/"s;
			train->key = "train_main_come_come";
		}
		train->totalTime = temp;
		train->isloop = false;
		train->quality = 0.4f;
		train->SetDrawingPriority(OrderLayer::SubTrain);
		train->Start();

	}
	trainInside->SetEnable(false);

}

void TrainSpriteObject::Come()
{
	trainState = ETrainState::Come;
	if (trainWheel)
	{
		if (std::filesystem::path path(L"Resource/Stage/wheel_"s + wtrainName + L"_come/");
			std::filesystem::exists(path))
		{
			trainWheel->OnDestroy();
			trainWheel->path = L"Resource/Stage/wheel_"s + wtrainName + L"_come/";
			trainWheel->key = "wheel_"s + trainName + "_come";
			trainWheel->totalTime = 5.0f;
			trainWheel->isloop = false;
			trainWheel->Start();
		}
		else
		{
			trainWheel->OnDestroy();
			trainWheel->path = L"Resource/Stage/wheel_come/";
			trainWheel->key = "wheel_come";
			trainWheel->totalTime = 5.0f;
			trainWheel->isloop = false;
			trainWheel->Start();
		}
	}

	if (train)
	{
		train->OnDestroy();
		train->path = L"Resource/Stage/train_"s + wtrainName + L"_come/"s;
		train->key = "train_"s + trainName + "_come";
		train->totalTime = 5.0f;
		train->isloop = false;
		train->Start();
		train->SetDrawingPriority(OrderLayer::Train);
	}

	if (trainInside)
	{
		if (std::filesystem::path path(L"Resource/Stage/wall_inside_"s + wtrainName + L"_come/"s);
			std::filesystem::exists(path))
		{
			trainInside->OnDestroy();
			trainInside->path = L"Resource/Stage/wall_inside_"s + wtrainName + L"_come/"s;
			trainInside->key = "wall_inside_"s + trainName + "_come";
			trainInside->totalTime = 5.0f;
			trainInside->isloop = false;
			trainInside->Start();
		}
		else
		{
			trainInside->OnDestroy();
			trainInside->path = L"Resource/Stage/wall_inside_main_come/";
			trainInside->key = "wall_inside_main_come";
			trainInside->totalTime = 5.0f;
			trainInside->isloop = false;
			trainInside->Start();
		}
		trainInside->SetEnable(true);
	}

	if (trainWall)
	{
		trainWall->OnDestroy();


		if (isCredit)
		{
			trainWall->path = L"Resource/Stage/train_credit_"s + wtrainName + L"_wall_come/";
			trainWall->key = "train_credit_"s + trainName + "_wall_come";
		}
		else
		{
			trainWall->path = L"Resource/Stage/train_"s + wtrainName + L"_wall_come/";
			trainWall->key = "train_"s + trainName + "_wall_come";
		}

		trainWall->totalTime = 5.0f;
		trainWall->isloop = false;
		trainWall->Start();
	}

}

void TrainSpriteObject::Default()
{
	if (trainWall && trainState == ETrainState::Init)
	{
		if (isCredit)
		{
			trainWall->OnDestroy();
			trainWall->path = L"Resource/Stage/train_credit_"s + wtrainName + L"_wall/";
			trainWall->key = "train_credit_" + trainName + "_wall";
			trainWall->totalTime = 0.1f;
			trainWall->isloop = false;
			trainWall->Start();

			trainWall->SetEnable(true);
			trainWall->GetComponent<SpriteRenderer>()->color.A = 0.0f;
			trainWall->AddRealTimeTimerFunction([=](bool lu)
												{
													trainWall->GetComponent<SpriteRenderer>()->color.A += (Time::GetDeltaTime() / 0.4f);
													if (lu)
													{
														trainWall->GetComponent<SpriteRenderer>()->color.A = 1.0f;
													}
												}, true, 0.4f, 1);
		}
		else
		{
			trainWall->OnDestroy();
			trainWall->path = L"Resource/Stage/train_"s + wtrainName + L"_wall/";
			trainWall->key = "train_" + trainName + "_wall";
			trainWall->totalTime = 0.1f;
			trainWall->isloop = false;
			trainWall->Start();
			trainWall->SetEnable(true);
			trainWall->GetComponent<SpriteRenderer>()->color.A = 1.0f;
		}
	}
	trainState = ETrainState::Default;

	if (trainWheel)
	{
		if (std::filesystem::path path(L"Resource/Stage/wheel_"s + wtrainName + L"/");
			std::filesystem::exists(path))
		{
			trainWheel->OnDestroy();
			trainWheel->path = L"Resource/Stage/wheel_"s + wtrainName + L"/";
			trainWheel->key = "wheel_"s + trainName;
			trainWheel->totalTime = 5.0f / 6.0f;
			trainWheel->isloop = true;
			trainWheel->Start();
		}
		else
		{
			trainWheel->OnDestroy();
			trainWheel->path = L"Resource/Stage/wheel/";
			trainWheel->key = "wheel";
			trainWheel->totalTime = 5.0f / 6.0f;
			trainWheel->isloop = true;
			trainWheel->Start();
		}
	}

	if (train)
	{
		if (std::filesystem::path path((L"Resource/Stage/train_"s + wtrainName + L"_curtain_open/").c_str());
			std::filesystem::exists(path) && !isReverse)
		{
			train->OnDestroy();
			train->path = L"Resource/Stage/train_"s + wtrainName + L"_curtain_open/";
			train->key = "train_"s + trainName + "_curtain_open";
			train->totalTime = 0.5f;
			train->isloop = false;
			train->Start();
		}
		else if (std::filesystem::path path((L"Resource/Stage/train_"s + wtrainName + L"/").c_str());
				 std::filesystem::exists(path))
		{
			train->OnDestroy();
			train->path = L"Resource/Stage/train_"s + wtrainName + L"/";
			train->key = "train_"s + trainName;
			train->totalTime = 0.01f;
			train->isloop = false;
			train->Start();
		}
		else
		{
			train->OnDestroy();
			train->path = L"Resource/Stage/base_train/";
			train->key = "train_"s + trainName + "_base";
			train->totalTime = 0.01f;
			train->isloop = false;
			train->Start();
		}
	}

	if (light)
	{
		if (std::filesystem::path path(L"Resource/Stage/Img_Main_BackLight_"s + wtrainName + L"/"s + wbackGroundName + L"_open_1/"s);
			std::filesystem::exists(path))
		{
			light->OnDestroy();
			light->path = L"Resource/Stage/Img_Main_BackLight_"s + wtrainName + L"/"s + wbackGroundName + L"_open_1/";
			light->key = backGroundName + "_open_1"; 
			light->pluskey = trainName + "_";
			light->totalTime = 0.5f;
			light->isloop = false;
			light->Start();
			wprintf(L"asdsdasds : %s \n", light->path.c_str());
		}
	}

	if (light2)
	{
		if (std::filesystem::path path(L"Resource/Stage/Img_Main_BackLight_"s + wtrainName + L"/"s + wbackGroundName + L"_open_2/");
			std::filesystem::exists(path))
		{
			light2->OnDestroy();
			light2->path = L"Resource/Stage/Img_Main_BackLight_"s + wtrainName + L"/"s + wbackGroundName + L"_open_2/";
			light2->key =  backGroundName + "_open_2";
			light2->pluskey = trainName + "_";
			light2->totalTime = 0.5f;
			light2->isloop = false;
			light2->Start();
		}
	}

	if (trainInside)
	{
		if (std::filesystem::path path(L"Resource/Stage/wall_inside_"s + wtrainName + L"/"s);
			std::filesystem::exists(path))
		{
			trainInside->OnDestroy();
			trainInside->path = L"Resource/Stage/wall_inside_"s + wtrainName + L"/"s;
			trainInside->key = "wall_inside_" + trainName;
			trainInside->totalTime = 0.01f;
			trainInside->isloop = false;
			trainInside->Start();
		}
		else
		{
			trainInside->OnDestroy();
			trainInside->path = L"Resource/Stage/wall_inside_main/";
			trainInside->key = "wall_inside_main";
			trainInside->totalTime = 0.01f;
			trainInside->isloop = false;
			trainInside->Start();
		}	
	}

	if (trainWall && isWallDisappear)
	{
		float t = 0.0f;
		trainWall->AddTimerFunction([this, t](bool lu) mutable
									{

										if (!lu)
										{
											t += Time::GetDeltaTime();
											trainWall->sprite->color.A = 1.0f - t;
										}
										else
										{
											trainWall->sprite->color.A = 1.0f;
											trainWall->SetEnable(false);
										}

									}, true, 1.0f, 1);
	}
}

void TrainSpriteObject::Out()
{
	trainState = ETrainState::Out;
	if (trainWheel)
	{
		if (std::filesystem::path path(L"Resource/Stage/wheel_"s + wtrainName + L"_out/");
			std::filesystem::exists(path))
		{
			trainWheel->OnDestroy();
			trainWheel->path = L"Resource/Stage/wheel_"s + wtrainName + L"_out/";
			trainWheel->key = "wheel_"s + trainName + "_out";
			trainWheel->totalTime = 5.0f;
			trainWheel->isloop = true;
			trainWheel->Start();
		}
		else
		{
			trainWheel->OnDestroy();
			trainWheel->path = L"Resource/Stage/wheel_out/";
			trainWheel->key = "wheel_out";
			trainWheel->totalTime = 5.0f;
			trainWheel->isloop = false;
			trainWheel->Start();
		}
	}

	if (train)
	{
		train->OnDestroy();
		train->path = L"Resource/Stage/train_"s + wtrainName + L"_out/";
		train->key = "train_"s + trainName + "_out";
		train->totalTime = 5.0f;
		train->isloop = false;
		train->Start();
	}

	if (trainInside)
	{

		if (std::filesystem::path path(L"Resource/Stage/wall_inside_"s + wtrainName + L"_out/"s);
			std::filesystem::exists(path))
		{
			trainInside->OnDestroy();
			trainInside->path = L"Resource/Stage/wall_inside_"s + wtrainName + L"_out/";
			trainInside->key = "wall_inside_"s + trainName + "_out";
			trainInside->totalTime = 5.0f;
			trainInside->isloop = false;
			trainInside->Start();
		}
		else
		{
			trainInside->OnDestroy();
			trainInside->path = L"Resource/Stage/wall_inside_main_out/";
			trainInside->key = "wall_inside_main_out";
			trainInside->totalTime = 5.0f;
			trainInside->isloop = false;
			trainInside->Start();
		}

	}

	if (trainWall)
	{
		trainWall->OnDestroy();

		if (isCredit)
		{
			trainWall->path = L"Resource/Stage/train_credit_"s + wtrainName + L"_wall_out/";
			trainWall->key = "train_credit_" + trainName + "_wall_out";
		}
		else
		{
			trainWall->path = L"Resource/Stage/train_"s + wtrainName + L"_wall_out/";
			trainWall->key = "train_" + trainName + "_wall_out";
		}

		trainWall->totalTime = 5.0f;
		trainWall->isloop = false;
		trainWall->Start();

		if (isWallDisappear)
		{
			trainWall->SetEnable(true);
			float t = 0.0f;
			trainWall->AddTimerFunction([this, t](bool lu) mutable
										{

											if (!lu)
											{
												t += Time::GetDeltaTime();
												trainWall->sprite->color.A = t;
											}
											else
											{
												trainWall->sprite->color.A = 1.0f;
											}

										}, true, 1.0f, 1);
		}

	}

	if (light)
	{
		light->Kill();
		light = nullptr;
	}
	if (light2)
	{
		light2->Kill();
		light2 = nullptr;
	}
}

void TrainSpriteObject::OutOut()
{
	trainState = ETrainState::OutOut;

	if (train)
	{
		if (std::filesystem::path path(L"Resource/Stage/train_"s + wtrainName + L"_out_out/"s);
			std::filesystem::exists(path))
		{
			train->OnDestroy();
			train->path = L"Resource/Stage/train_"s + wtrainName + L"_out_out/"s;
			train->key = "train_"s + trainName + "_out_out";
			train->totalTime = temp;
			train->isloop = false;

			train->Start();
		}
		else
		{
			train->OnDestroy();
			train->path = L"Resource/Stage/train_main_out_out/"s;
			train->key = "train_main_out_out";
			train->totalTime = isCredit ? 0.01f : temp;
			train->isloop = false;
			train->Start();
		}
		train->SetDrawingPriority(OrderLayer::SubTrain);
	}

	AddTimerFunction([this](bool lu)
					 {
						 if (!isReverse)
						 {
							 Kill();
						 }
					 }, false, temp, 1);
}

void TrainSpriteObject::WaitAndCome()
{
	AddTimerFunction([this](bool lu)
					 {
						 Come();
					 }, false, 0.5f, 1);
}

void TrainSpriteObject::WaitAndComeCome()
{
	AddTimerFunction([this](bool lu)
					 {
						 ComeCome();
					 }, false, 5.0f - temp, 1);
}

void TrainSpriteObject::CurtainColseAndOut()
{
	std::filesystem::path path((L"Resource/Stage/train_"s + wtrainName + L"_curtain_open/").c_str());
	if (std::filesystem::exists(path))
	{
		if (train)
		{
			train->OnDestroy();
			train->path = L"Resource/Stage/train_"s + wtrainName + L"_curtain_open/";
			train->key = "train_"s + trainName + "_curtain_open";
			train->totalTime = 0.5f;
			train->isloop = false;
			train->isReverse = !isReverse;
			train->Start();
		}

		if (light)
		{
			light->OnDestroy();
			light->path = L"Resource/Stage/Img_Main_BackLight_"s + wtrainName + L"/"s + wbackGroundName + L"_open_1/";
			light->key = backGroundName + "_open_1";
			light->totalTime = 0.5f;
			light->isReverse = true;
			light->isloop = false;
			light->Start();
		}
		if (light2)
		{
			light2->OnDestroy();
			light2->path = L"Resource/Stage/Img_Main_BackLight_"s + wtrainName + L"/"s + wbackGroundName + L"_open_2/";
			light2->key = backGroundName + "_open_2";
			light2->totalTime = 0.5f;
			light2->isReverse = true;
			light2->isloop = false;
			light2->Start();
		}
	}
	AddTimerFunction([this](bool lu)
					 {
						 train->isReverse = isReverse;
						 Out();
					 }, false, 0.5f, 1);
}

void TrainSpriteObject::WaitAndOutOut()
{
	AddTimerFunction([this](bool lu)
					 {
						 OutOut();
					 }, false, 0.5f, 1);
}

void TrainSpriteObject::SetReverse(bool isReverse)
{
	this->isReverse = isReverse;
	if (train)
	{
		train->isReverse = isReverse;
	}
	if (light)
	{
		light->isReverse = isReverse;
	}
	if (light2)
	{
		light2->isReverse = isReverse;
	}
	if (trainWheel)
	{
		trainWheel->isReverse = isReverse;
	}
	if (trainInside)
	{
		trainInside->isReverse = isReverse;
	}
	if (trainWall)
	{
		trainWall->isReverse = isReverse;
	}
}
