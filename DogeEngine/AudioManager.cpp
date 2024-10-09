#include "AudioManager.h"
#include "ResourceManager.h"
#include "Core.h"



float AudioManager::MASTER_VOLUME = 1.0f;


AudioManager::AudioManager()
{
}
AudioManager::~AudioManager()
{
}


void AudioManager::SetSound(std::string key, bool loop)
{
	if (channelSetted == true)
	{
		Stop();
	}

	currentSound = ResourceManager::GetAudio(key);
	if (loop == true)
	{
		currentSound->setMode(FMOD_LOOP_NORMAL);
	}
	else if (loop == false)
	{
		currentSound->setMode(FMOD_LOOP_OFF);
	}
}

void AudioManager::SetBankSound(std::string bankKey, std::string soundKey, bool loop)
{
	if (channelSetted == true)
	{
		Stop();
	}

	currentSound = ResourceManager::GetSoundBank(bankKey)->GetSound(soundKey);
	if (loop == true)
	{
		currentSound->setMode(FMOD_LOOP_NORMAL);
	}
	else if (loop == false)
	{
		currentSound->setMode(FMOD_LOOP_OFF);
	}
}

void AudioManager::Play()
{
	if (currentSound == nullptr) return;

	// 알아서 사용 가능한 채널이 배정됨
	DG::Core::GetInstance().GetFMODSystem()->playSound(currentSound, 0, false, &channel);
	channel->setVolume(volume * MASTER_VOLUME);
	channelSetted = true;
}
void AudioManager::Play(std::string key, bool loop)
{
	SetSound(key, loop);
	Play();
}

void AudioManager::Pause()
{
	if (channelSetted == true)
	{
		channel->setPaused(true);
	}
}
void AudioManager::Resume()
{
	if (channelSetted == true)
	{
		channel->setPaused(false);
	}
}
void AudioManager::Stop()
{
	if (channelSetted == true)
	{
		channel->stop();
		channelSetted = false;
	}
}




void AudioManager::SetVolume(float volume)
{
	if (volume >= 1)
	{
		this->volume = 1.0f;
		if (channelSetted == true)
		{
			channel->setVolume(1.0f * MASTER_VOLUME);
		}
	}
	else if (volume <= 0)
	{
		this->volume = 0.0f;
		if (channelSetted == true)
		{
			channel->setVolume(0);
		}
	}
	else
	{
		this->volume = volume;
		if (channelSetted == true)
		{
			channel->setVolume(volume * MASTER_VOLUME);
		}
	}
}
void AudioManager::VolumeDown(float amount)
{
	if (volume - amount < 0)
	{
		volume = 0.0f;
	}
	else
	{
		volume -= amount;
	}

	if (channelSetted == true)
	{
		channel->setVolume(volume * MASTER_VOLUME);
	}
}
void AudioManager::VolumeUp(float amount)
{
	if (volume + amount > 1.0f)
	{
		volume = 1.0f;
	}
	else
	{

		volume += amount;
	}

	if (channelSetted == true)
	{
		channel->setVolume(volume * MASTER_VOLUME);
	}
}
void AudioManager::Mute()
{
	volume = 0.0f;
	if (channelSetted == true)
	{
		channel->setVolume(0.0f);
	}
}
void AudioManager::SetPitch(float pitch)
{
	if (channelSetted == true)
	{
		channel->setPitch(pitch);
	}
}