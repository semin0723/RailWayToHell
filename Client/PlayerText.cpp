#include "PlayerText.h"

PlayerText& PlayerTextManager::GetText(int textID)
{
	if (GetInstance().textMap.empty())
	{
		std::vector<PlayerText> PlayerTextVector;
		DataAsset::LoadDataAsset<PlayerText>("Resource/PlayerText.csv", PlayerTextVector);
		

		for (size_t i = 0; i < PlayerTextVector.size(); i++)
		{
			GetInstance().textMap.emplace(PlayerTextVector[i].BubbleID, std::move(PlayerTextVector[i]));
		}



	}
	return GetInstance().textMap[textID];
}

PlayerTextManager& PlayerTextManager::GetInstance()
{
	static PlayerTextManager instance;
	return instance;
}
