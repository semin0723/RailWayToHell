#include "TrashAnimation.h"
#include <thread>

using namespace std::string_literals;

void TrashAnimation::Awake()
{
	sprite = AddComponent<SpriteRenderer>();
	quality = 1.0f;
}

void TrashAnimation::Start()
{
	std::wifstream sstream2(path + L"LastIndex.txt"s);
	sstream2 >> maxIndex;
	



	elapsedTime = 0.0f;

	if (!isReverse)
	{
		index = 0;
		unloadIndex = 0;
		loadIndex = 0;
	}
	else
	{
		index = maxIndex + 1;
		unloadIndex = maxIndex + 1;
		loadIndex = maxIndex + 1;
	}



	//printf("%s : %d \n", key.c_str(), maxIndex);
	//wprintf(L"%s\n", path.c_str());
	LoadSprite();
	LoadSprite();
	LoadSprite();
	LoadSprite();
	NextSprite();

}

void TrashAnimation::Update()
{
	elapsedTime += Time::GetDeltaTime();

	float limitTime = totalTime / (float)maxIndex;
	while (elapsedTime > limitTime)
	{
		elapsedTime -= limitTime;
		LoadSprite();
		NextSprite();
		if (!isloop)
		{
			UnLoadSprite();
		}
	}



}

void TrashAnimation::OnDestroy()
{
	while (!IsUnLoadMaxIndex())
	{
		UnLoadSprite();
	}

	std::wstringstream sstream;
	std::wstring wkey(key.begin(), key.end());
	sstream << path << wkey << L" (" << NextUnLoadIndex() << L").png";

	ResourceManager::DeleteImage(pluskey + key + std::to_string(unloadIndex));

	unloadIndex = 0;
	loadIndex = 0;

}

void TrashAnimation::UnLoadSprite()
{
	if (IsUnLoadMaxIndex())
	{
		return;
	}
	std::wstringstream sstream;
	std::wstring wkey(key.begin(), key.end());
	sstream << path << wkey << L" (" << NextUnLoadIndex() << L").png";

	ResourceManager::DeleteImage(pluskey + key + " (" + std::to_string(unloadIndex) + ").png");
}

void TrashAnimation::LoadSprite()
{
	if(IsLoadMaxIndex())
	{
		return;
	}
	std::wstringstream sstream;
	std::wstring wkey(key.begin(), key.end());
	sstream << path << wkey << L" (" << NextLoadIndex() << L").png";
	//printf("%s \n", (key + std::to_string(loadIndex)).c_str());

	if (IsLoadMaxIndex())
	{
		ResourceManager::AddImageByPathThread(pluskey + key + " (" + std::to_string(loadIndex) + ").png", sstream.str().c_str(), 1.0f);
		//ResourceManager::AddImageByPath(key + std::to_string(loadIndex), sstream.str().c_str(), 1.0f);
	}
	else
	{
		ResourceManager::AddImageByPathThread(pluskey + key + " (" + std::to_string(loadIndex) + ").png", sstream.str().c_str(), quality);
		//ResourceManager::AddImageByPath(key + std::to_string(loadIndex), sstream.str().c_str(), quality);
	}

}

void TrashAnimation::NextSprite()
{
	if (IsMaxIndex())
	{
		if (isloop)
		{
			InitIndex();
		}
		else
		{
			sprite->SetImage(pluskey + key + " (" + std::to_string(index) + ").png");
			sprite->SetSizeAuto();
			sprite->SetSizeByWidth(1920.0f);
			return;
		}
	}

	NextIndex();
	sprite->SetImage(pluskey + key + " (" + std::to_string(index) + ").png");
	sprite->SetSizeAuto();
	sprite->SetSizeByWidth(1920.0f);
	//sprite->SetSize(1920, 1080);
}

bool TrashAnimation::IsMaxIndex()
{
	return !isReverse ?
		index >= maxIndex :
		index <= 1;
}

bool TrashAnimation::IsLoadMaxIndex()
{
	return !isReverse ?
		loadIndex >= maxIndex :
		loadIndex <= 1;
}

bool TrashAnimation::IsUnLoadMaxIndex()
{
	return !isReverse ?
		unloadIndex >= loadIndex  - 1:
		unloadIndex <= 1;
}

int TrashAnimation::NextIndex()
{
	return !isReverse ?
		++index :
		--index;
}

int TrashAnimation::NextLoadIndex()
{
	return !isReverse ?
		++loadIndex :
		--loadIndex;
}

int TrashAnimation::NextUnLoadIndex()
{
	return !isReverse ?
		++unloadIndex :
		--unloadIndex;
}

void TrashAnimation::InitIndex()
{
	index = !isReverse ?
		0 :
		maxIndex;
}
