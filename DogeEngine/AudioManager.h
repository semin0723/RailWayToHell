#pragma once
#include <string>
#include "AudioFramework.h"
#include "Component.h"


class AudioManager : public Component
{
	static float MASTER_VOLUME;

	FMOD::Channel* channel = nullptr;
	FMOD::Sound* currentSound = nullptr;
	float volume = 1.0f;

	bool channelSetted = false;
public:
	AudioManager();
	~AudioManager();

	void SetSound(std::string key, bool loop = false);
	void SetBankSound(std::string bankKey, std::string soundKey, bool loop = false);

	void Play();
	void Play(std::string key, bool loop = false);

	// 현재 채널의 재생을 잠시 중단합니다.
	void Pause();
	// 중단되었던 현재 채널의 재생을 재개합니다.
	void Resume();
	// 채널을 해제하여 재생을 중지합니다. 다시 플레이하려면 Play()를 호출하여 새 채널을 할당받아야 합니다.
	void Stop();

	void SetVolume(float volume);
	void VolumeDown(float amount);
	void VolumeUp(float amount);
	void Mute();

	void SetPitch(float pitch);
};