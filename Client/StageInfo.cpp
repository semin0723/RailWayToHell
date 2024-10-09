#include "StageInfo.h"

void StageInfo::GetSpawnPos(std::vector<int>& outPos, std::vector<int>& outWeight, int currentTurn)
{
	if (currentTurn == 1)
	{
		outPos = StartSpawnPos;
		outWeight = StartSpawnWeight;
	}
	else if (currentTurn == WaveSpawnTurn)
	{
		outPos = WaveSpawnPos;
		outWeight = WaveSpawnWeight;
	}
}

bool StageInfo::IsWaveTurn(int turn)
{
	if (turn == WaveSpawnTurn) {
		return true;
	}
	return false;
}

int StageInfo::GetTrackingSpawn(int currentTurn)
{
	if (currentTurn == TrackingSpawnTurn1)
	{
		return TrackingSpawnID1;
	}
	if (currentTurn == TrackingSpawnTurn2)
	{
		return TrackingSpawnID2;
	}
	if (currentTurn == TrackingSpawnTurn3)
	{
		return TrackingSpawnID3;
	}
	if (currentTurn == TrackingSpawnTurn4)
	{
		return TrackingSpawnID4;
	}
	return -1;
}

StageInfo2::~StageInfo2()
{
	for (auto& item : WaveSpawnInfos)
	{
		delete item;
	}
	for (auto& item : TrackingSpawnInfos)
	{
		delete item;
	}
}

void StageInfo2::GetSpawnPos(std::vector<int>& outPos, std::vector<int>& outWeight, int currentTurn)
{
	auto findIter = WaveSpawnInfoMap.find(currentTurn);
	if (findIter != WaveSpawnInfoMap.end())
	{
		outPos = findIter->second->SpawnPos;
		outWeight = findIter->second->SpawnWeight;
	}
}

bool StageInfo2::IsWaveTurn(int turn)
{
	auto findIter = WaveSpawnInfoMap.find(turn);
	return (findIter != WaveSpawnInfoMap.end());
}

int StageInfo2::GetTrackingSpawn(int currentTurn)
{
	auto findIter = TrackingSpawnInfoMap.find(currentTurn);
	if (findIter != TrackingSpawnInfoMap.end())
	{
		return findIter->second->SpawnID;
	}
	return -1;
}

void StageInfo2::BePrepare()
{
	for (auto& item : WaveSpawnInfos)
	{
		WaveSpawnInfoMap.emplace(item->SpawnTurn, item);
	}
	for (auto& item : TrackingSpawnInfos)
	{
		TrackingSpawnInfoMap.emplace(item->SpawnTurn, item);
	}
}
