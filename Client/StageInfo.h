#pragma once
#include "Engine.h"

class IStageInfo
{
public:
	virtual void GetSpawnPos(std::vector<int>& outPos, std::vector<int>& outWeight, int currentTurn) = 0;
	virtual bool IsWaveTurn(int turn) = 0;
	virtual const int GetStage() = 0;
	virtual int GetTrackingSpawn(int currentTurn) = 0;
	virtual int GetMaxTurn() = 0;
};

class StageInfo : public DataAsset, public IStageInfo
{
public:
	virtual void GetSpawnPos(std::vector<int>& outPos, std::vector<int>& outWeight, int currentTurn) override;
	virtual bool IsWaveTurn(int turn) override;
	virtual const int GetStage() override { return Stage; }
	virtual int GetTrackingSpawn(int currentTurn) override;
	virtual int GetMaxTurn() override { return StageMaxTurn; }

private:
	int Stage;
	int StageMaxTurn;

	std::vector<int> StartSpawnPos;
	std::vector<int> StartSpawnWeight;

	int WaveSpawnTurn;
	std::vector<int> WaveSpawnPos;
	std::vector<int> WaveSpawnWeight;

	int TrackingSpawnTurn1;
	int TrackingSpawnID1;
	int TrackingSpawnTurn2;
	int TrackingSpawnID2;
	int TrackingSpawnTurn3;
	int TrackingSpawnID3;
	int TrackingSpawnTurn4;
	int TrackingSpawnID4;


#define StageInfoReflect(x)	\
	x(Stage)				\
	x(StageMaxTurn)				\
	x(StartSpawnPos)			\
	x(StartSpawnWeight)			\
	x(WaveSpawnTurn)			\
	x(WaveSpawnPos)			\
	x(WaveSpawnWeight)			\
	x(TrackingSpawnTurn1)			\
	x(TrackingSpawnID1)			\
	x(TrackingSpawnTurn2)			\
	x(TrackingSpawnID2)			\
	x(TrackingSpawnTurn3)			\
	x(TrackingSpawnID3)			\
	x(TrackingSpawnTurn4)			\
	x(TrackingSpawnID4)			\



	GENERATED_BODY(StageInfo, DataAsset)
};

class SpawnInfo : public ReflectObject
{
public:
	int SpawnTurn;
	std::vector<int> SpawnPos;
	std::vector<int> SpawnWeight;

#define SpawnInfoReflect(x)	\
	x(SpawnTurn)			\
	x(SpawnPos)			\
	x(SpawnWeight)

	GENERATED_BODY(SpawnInfo, ReflectObject)
};

class TrackingSpawnInfo : public ReflectObject
{
public:
	int SpawnTurn;
	int SpawnID;
#define TrackingSpawnInfoReflect(x)	\
	x(SpawnTurn)					\
	x(SpawnID)

	GENERATED_BODY(TrackingSpawnInfo, ReflectObject)
};

class StageInfo2 : public ReflectObject, public IStageInfo
{
public:
	virtual ~StageInfo2();
	virtual void GetSpawnPos(std::vector<int>& outPos, std::vector<int>& outWeight, int currentTurn) override;
	virtual bool IsWaveTurn(int turn) override;
	virtual const int GetStage() override { return Stage; }
	virtual int GetTrackingSpawn(int currentTurn) override;
	virtual int GetMaxTurn() override { return StageMaxTurn; }

	void BePrepare();

public:
	int Stage;
	int StageMaxTurn;
	std::vector<SpawnInfo*> WaveSpawnInfos;
	std::vector<TrackingSpawnInfo*> TrackingSpawnInfos;

	std::map<int, SpawnInfo*> WaveSpawnInfoMap;
	std::map<int, TrackingSpawnInfo*> TrackingSpawnInfoMap;
	
#define StageInfo2Reflect(x)	\
	x(Stage)					\
	x(StageMaxTurn)					\
	x(WaveSpawnInfos)			\
	x(TrackingSpawnInfos)			\



	GENERATED_BODY(StageInfo2, ReflectObject)
};


class StructureInfo : public DataAsset
{
public:
	int Stage;
	std::vector<int> Chandelier;
	std::vector<int> GunPowder;
#define StructureInfoReflect(x)	\
	x(Stage)					\
	x(Chandelier)					\
	x(GunPowder)			\



	GENERATED_BODY(StructureInfo, DataAsset)
};


class TrainInfo : public DataAsset
{
public:
	int Stage;
	std::string TrainName;
	std::string BackGround;

#define TrainInfoReflect(x)	\
	x(Stage)					\
	x(TrainName)					\
	x(BackGround)					\

	GENERATED_BODY(TrainInfo, DataAsset)
};

