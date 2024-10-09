#pragma once
#include "Component.h"
#include "Animation.h"

// GIF로 이미지를 불러온 경우 GIF 포맷의 특성상 2프레임부터는 변한 부분만 덧그리는 방식일 수 있어서 이미지 크기가 프레임마다 제각각일 수 있다.

// 내부에 elapsedTime 변수를 두고 특정 시간마다 SpriteRenderer의 스프라이트를 교체해주는 역할
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