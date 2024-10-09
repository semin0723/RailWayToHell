#include "AudioFramework.h"
#include <fstream>
#include <sstream>
#include "Core.h"
#include "ResourceManager.h"

std::string ToLower(const std::string& str)
{
	std::string result = "";

	for (char ch : str)
	{
		// Convert each character to lowercase using tolower 
		result += tolower(ch);
	}

	return result;
}

FmodSoundBank::FmodSoundBank(FMOD::Sound* stream, std::wstring subFilepath) : stream(stream)
{

	std::ifstream fileStream(subFilepath);

	if (fileStream.is_open())
	{
		std::string line;
		while (std::getline(fileStream, line))
		{
			std::stringstream liineStream(line);

			std::string token1;
			std::string token2;
			std::string token3;


			std::getline(liineStream, token1, ' ');
			std::getline(liineStream, token2, ' ');
			std::getline(liineStream, token3, ' ');
			if (!token3.empty())
			{
				stringTable[ToLower(token2)] = std::stoi(token3);
			}
		}
	}
	

}

FMOD::Sound* FmodSoundBank::GetSound(std::string soundName)
{
	FMOD_RESULT result;
	auto finditer = stringTable.find(ToLower(soundName));
	FMOD::Sound* resultSound = nullptr;
	if (finditer != stringTable.end())
	{

		result = stream->getSubSound(finditer->second, &resultSound);
		assert(result == FMOD_OK);
	}

	return resultSound;
}

FMOD::Channel* AudioFramework::Play_Sound(FMOD::Sound* sound, bool isloop)
{
	FMOD::Channel* channel;

	if (isloop == true)
	{
		sound->setMode(FMOD_LOOP_NORMAL);
	}
	else
	{
		sound->setMode(FMOD_LOOP_OFF);
	}

	DG::Core::GetInstance().GetFMODSystem()->playSound(sound, 0, false, &channel);
	return channel;
}

FMOD::Channel* AudioFramework::Play_BankSound(std::string bankName, std::string soundName, bool isloop)
{
	return Play_Sound(ResourceManager::GetSoundBank(bankName)->GetSound(soundName), isloop);
}
