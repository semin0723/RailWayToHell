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

	// ���� ä���� ����� ��� �ߴ��մϴ�.
	void Pause();
	// �ߴܵǾ��� ���� ä���� ����� �簳�մϴ�.
	void Resume();
	// ä���� �����Ͽ� ����� �����մϴ�. �ٽ� �÷����Ϸ��� Play()�� ȣ���Ͽ� �� ä���� �Ҵ�޾ƾ� �մϴ�.
	void Stop();

	void SetVolume(float volume);
	void VolumeDown(float amount);
	void VolumeUp(float amount);
	void Mute();

	void SetPitch(float pitch);
};