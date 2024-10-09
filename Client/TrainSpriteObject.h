#pragma once
#include "Engine.h"
class TrashAnimation;

namespace ETrainState
{
	enum Type
	{
		Init,
		ComeCome,
		Come,
		Default,
		Out,
		OutOut,
		Max
	};
}

class TrainSpriteObject : public Object
{
public:
	void Awake() override;
	void Init(std::string trainName, std::string backGroundName);

	void ComeCome();
	void Come();
	void Default();
	void Out();
	void OutOut();

	void WaitAndCome();
	void WaitAndComeCome();
	void CurtainColseAndOut();
	void WaitAndOutOut();

	void SetReverse(bool isReverse);
	void SetIsWallDisappear(bool isWallDisappear) { this->isWallDisappear = isWallDisappear; }
	void SetIsCredit(bool isCredit) { this->isCredit = isCredit; }

	ETrainState::Type GetTrainState() { return trainState; }

public:
	TrashAnimation* train;
	TrashAnimation* light;
	TrashAnimation* light2;
	TrashAnimation* trainWheel;
	TrashAnimation* trainInside;
	TrashAnimation* trainWall;

	std::string trainName;
	std::wstring wtrainName;
	std::string backGroundName;
	std::wstring wbackGroundName;

	bool isWallDisappear;
	bool isCredit;
	bool isReverse;

	ETrainState::Type trainState;
};
