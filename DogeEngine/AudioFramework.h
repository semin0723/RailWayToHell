#pragma once

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <map>
#include <string>

class SoundBank;
class SoundBankChannel;

struct AudioFramework
{
	static FMOD::Channel* Play_Sound(FMOD::Sound* sound, bool isloop = false);
	static FMOD::Channel* Play_BankSound(std::string bankName, std::string soundName, bool isloop = false);
};



/**
 * @brief .fsb파일
 */
class FmodSoundBank
{
public:
	FmodSoundBank(FMOD::Sound* stream, std::wstring subFilepath);
	FmodSoundBank(const FmodSoundBank&) = delete;
	FmodSoundBank(FmodSoundBank&&) = delete;
	FmodSoundBank& operator=(const FmodSoundBank&) = delete;
	FmodSoundBank& operator=(const FmodSoundBank&&) = delete;
	~FmodSoundBank() { stream->release(); }



	FMOD::Sound* GetSound(std::string soundName);

private:
	FMOD::Sound* stream;
	std::map<std::string, int> stringTable;
};


/**
 * @brief Fmod Studio로 만든 .Bank파일
 */
class SoundBank
{
public:
	SoundBank(FMOD::Studio::Bank* bank) : bank(bank) {}
	SoundBank(const SoundBank&) = delete;
	SoundBank(SoundBank&&) = delete;
	SoundBank& operator=(const SoundBank&) = delete;
	SoundBank& operator=(const SoundBank&&) = delete;
	~SoundBank() { bank->unload(); }

	SoundBankChannel* Play(const char* eventName);


private:
	FMOD::Studio::EventDescription* eventDescription;
	FMOD::Studio::Bank* bank;
};

/**
 * @brief .Bank파일속 이벤트
 */
class SoundBankChannel
{
public:
	SoundBankChannel(FMOD::Studio::EventInstance* eventInstance) : eventInstance(eventInstance) {}
	~SoundBankChannel()
	{
		eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}
	SoundBankChannel(const SoundBankChannel&) = delete;
	SoundBankChannel(SoundBankChannel&&) = delete;

public:
	template<class T>
	void SetParameter(const char* paramName, T val)
	{
		FMOD_RESULT result;
		FMOD::Studio::EventDescription* eventDescription;
		result = eventInstance->getDescription(&eventDescription);
		FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
		result = eventDescription->getParameterDescriptionByName(paramName, &paramDesc);
		FMOD_STUDIO_PARAMETER_ID parameterID = paramDesc.id;

		eventInstance->setParameterByID(parameterID, val);
	}

	void Stop()
	{
		eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}

	void Play()
	{
		eventInstance->start();
	}
private:
	FMOD::Studio::EventInstance* eventInstance;
};