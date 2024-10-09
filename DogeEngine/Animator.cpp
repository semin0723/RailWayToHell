#include "Animator.h"
#include "Object.h"
#include "SpriteRenderer.h"
#include "defines.h"

Animator::Animator()
{
}
Animator::~Animator()
{
	// 애니메이션은 스택변수로 다른 곳에서 만드니까 delete 하면 안 됨
}
void Animator::SetAnimation(Animation& _animation, bool update_size_to_first_image)
{
	SpriteRenderer* renderer = GetOwner()->GetComponent<SpriteRenderer>();
	if (renderer == nullptr) return;

	if (useAtlas == true)
	{
		printf("아틀라스 스프라이트 모드 끔");
		useAtlas = false;
	}

	currentAnimation = &_animation;
	ComPtr<ID2D1Bitmap> bitmap = currentAnimation->InitAnimation();
	if (bitmap == nullptr) return;

	renderer->SetImage(bitmap);

	if (update_size_to_first_image == true)
	{
		renderer->SetSizeAuto();
	}
}
void Animator::SetAnimationWhenPrevAnimationDone(Animation& animation, bool update_size_to_first_image)
{
	if (currentAnimation == nullptr)
		return SetAnimation(animation, update_size_to_first_image);

	SetLoop(false);
	nextAnimation = &animation;
	update_size_to_first_image_when_animation_change = update_size_to_first_image;
}
Animation* Animator::GetCurrentAnimation()
{
	return currentAnimation;
}
void Animator::SetAnimation(AtlasAnimation& _animation, bool update_size_to_first_image)
{
	SpriteRenderer* renderer = GetOwner()->GetComponent<SpriteRenderer>();
	if (renderer == nullptr) return;

	if (useAtlas == false)
	{
		printf("아틀라스 스프라이트 모드 켬");
		useAtlas = true;
	}

	currentAtlasAnimation = &_animation;
	auto pair = currentAtlasAnimation->InitAnimation();

	if (pair.first == nullptr) return;
	renderer->SetImage(pair.first, pair.second);

	if (update_size_to_first_image)
	{
		renderer->SetSizeAuto();
	}
}
void Animator::SetAnimationWhenPrevAnimationDone(AtlasAnimation& animation, bool update_size_to_first_image)
{
	if (currentAtlasAnimation == nullptr)
		return SetAnimation(animation, update_size_to_first_image);

	SetLoop(false);
	nextAtlasAnimation = &animation;
	update_size_to_first_image_when_animation_change = update_size_to_first_image;
}
AtlasAnimation* Animator::GetCurrentAtlasAnimation()
{
	return currentAtlasAnimation;
}
void Animator::UpdateFrameByTime()
{
	if (isRunOnTime == false) return;

	SpriteRenderer* renderer = GetOwner()->GetComponent<SpriteRenderer>();
	if (renderer == nullptr) return;

	if (useAtlas == true)
	{
		if (currentAtlasAnimation == nullptr) return;
		if (currentAtlasAnimation->IsImageExist() == false) return;

		auto pair = currentAtlasAnimation->UpdateFrameByTime();
		if (nextAtlasAnimation != nullptr && currentAtlasAnimation->IsLastFrameAndElapsed())
		{
			SetAnimation(*nextAtlasAnimation, update_size_to_first_image_when_animation_change);
			nextAtlasAnimation = nullptr;
			return;
		}

		if (renderer->GetImage().Get() != pair.first.Get())
		{
			Assert(false, "atlas image changed unexpectedly");
		}
		else
		{
			renderer->SetImage(pair.first, pair.second);
		}
	}
	else if (useAtlas == false)
	{
		if (currentAnimation == nullptr) return;
		if (currentAnimation->IsImageExist() == false) return;

		ComPtr<ID2D1Bitmap> bitmap = currentAnimation->UpdateFrameByTime();
		if (nextAnimation != nullptr && currentAnimation->IsLastFrameAndElapsed())
		{
			SetAnimation(*nextAnimation, update_size_to_first_image_when_animation_change);
			nextAnimation = nullptr;
			return;
		}

		if (renderer->GetImage().Get() != bitmap.Get())
		{
			renderer->SetImage(bitmap);
		}
	}
}
void Animator::NextFrame()
{
	SpriteRenderer* renderer = GetOwner()->GetComponent<SpriteRenderer>();
	if (renderer == nullptr) return;

	if (useAtlas == true)
	{
		if (currentAtlasAnimation == nullptr) return;
		if (currentAtlasAnimation->IsImageExist() == false) return;

		auto pair = currentAtlasAnimation->NextFrame();
		if (nextAtlasAnimation != nullptr && currentAtlasAnimation->IsLastFrameAndElapsed())
		{
			SetAnimation(*nextAtlasAnimation, update_size_to_first_image_when_animation_change);
			nextAtlasAnimation = nullptr;
			return;
		}

		if (renderer->GetImage().Get() != pair.first.Get())
		{
			renderer->SetImage(pair.first, pair.second);
		}
	}
	else if (useAtlas == false)
	{
		if (currentAnimation == nullptr) return;
		if (currentAnimation->IsImageExist() == false) return;

		ComPtr<ID2D1Bitmap> bitmap = currentAnimation->NextFrame();
		if (nextAnimation != nullptr && currentAnimation->IsLastFrameAndElapsed())
		{
			SetAnimation(*nextAnimation, update_size_to_first_image_when_animation_change);
			nextAnimation = nullptr;
			return;
		}

		if (renderer->GetImage().Get() != bitmap.Get())
		{
			renderer->SetImage(bitmap);
		}
	}
}
void Animator::SetCurrentFrameIndex(int index)
{
	if (useAtlas == false)
	{
		currentAnimation->SetFrameIndex(index);
	}
	else if (useAtlas == true)
	{
		currentAtlasAnimation->SetFrameIndex(index);
	}
}
int Animator::GetFrameIndex()
{
	if (useAtlas == false)
	{
		return currentAnimation->GetFrameIndex();
	}
	else if (useAtlas == true)
	{
		return currentAtlasAnimation->GetFrameIndex();
	}
}
bool Animator::IsLastFrame()
{
	if (useAtlas == false)
	{
		return currentAnimation->IsLastFrame();
	}
	else if (useAtlas == true)
	{
		return currentAtlasAnimation->IsLastFrame();
	}
}
bool Animator::IsAnimationDone()
{
	if (useAtlas == false)
	{
		if (currentAnimation->loop == true) return false;
		return currentAnimation->IsLastFrameAndElapsed();
	}
	else if (useAtlas == true)
	{
		if (currentAtlasAnimation->loop == true) return false;
		return currentAtlasAnimation->IsLastFrameAndElapsed();
	}
}
float Animator::GetInterval()
{
	if (useAtlas == false)
	{
		return currentAnimation->GetInterval();
	}
	else if (useAtlas == true)
	{
		return currentAtlasAnimation->GetInterval();
	}
}
void Animator::SetInterval(float interval)
{
	if (useAtlas == false)
	{
		if (currentAnimation == nullptr) return;
		currentAnimation->SetInterval(interval);
	}
	else if (useAtlas == true)
	{
		if (currentAtlasAnimation == nullptr) return;
		currentAtlasAnimation->SetInterval(interval);
	}
}
void Animator::SetLoop(bool loop)
{
	if (useAtlas == true)
	{
		if (currentAtlasAnimation == nullptr) return;
		currentAtlasAnimation->loop = loop;
	}
	else
	{
		if (currentAnimation == nullptr) return;
		currentAnimation->loop = loop;
	}
}
bool Animator::GetLoopSetting()
{
	if (useAtlas == true)
	{
		if (currentAtlasAnimation == nullptr) return false;
		return currentAtlasAnimation->loop;
	}
	else
	{
		if (currentAnimation == nullptr) return false;
		return currentAnimation->loop;
	}
}