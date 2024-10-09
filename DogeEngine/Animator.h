#pragma once
#include "Component.h"
#include "Animation.h"

// GIF�� �̹����� �ҷ��� ��� GIF ������ Ư���� 2�����Ӻ��ʹ� ���� �κи� ���׸��� ����� �� �־ �̹��� ũ�Ⱑ �����Ӹ��� �������� �� �ִ�.

// ���ο� elapsedTime ������ �ΰ� Ư�� �ð����� SpriteRenderer�� ��������Ʈ�� ��ü���ִ� ����
class Animator : public Component
{
	Animation* currentAnimation = nullptr;
	Animation* nextAnimation = nullptr;
	AtlasAnimation* currentAtlasAnimation = nullptr;
	AtlasAnimation* nextAtlasAnimation = nullptr;
	bool update_size_to_first_image_when_animation_change = false;
public:
	bool always_use_image_raw_size = false;
	bool isRunOnTime = true;
	bool useAtlas = false;

	Animator();
	~Animator();

	void SetAnimation(Animation& animation, bool update_size_to_first_image = false);
	void SetAnimationWhenPrevAnimationDone(Animation& animation, bool update_size_to_first_image = false);
	Animation* GetCurrentAnimation();

	void SetAnimation(AtlasAnimation& animation, bool update_size_to_first_image = false);
	void SetAnimationWhenPrevAnimationDone(AtlasAnimation& animation, bool update_size_to_first_image = false);
	AtlasAnimation* GetCurrentAtlasAnimation();

	void UpdateFrameByTime();
	void NextFrame();

	void SetCurrentFrameIndex(int index);
	int GetFrameIndex();
	bool IsLastFrame();
	bool IsAnimationDone();

	float GetInterval();
	void SetInterval(float interval);

	void SetLoop(bool loop);
	bool GetLoopSetting();
};