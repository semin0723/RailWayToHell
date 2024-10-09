#include <string>
#include <vector>
#include "Animation.h"
#include "ResourceManager.h"
#include "Time.h"

using DG::Frame;

void Animation::AddFrame(std::string key)
{
	ComPtr<ID2D1Bitmap> bitmap = ResourceManager::GetImage(key);

	if (bitmap == nullptr) return;

	Frame* frame = new Frame(bitmap);
	frames.push_back(frame);

	if (frames.size() == 1) InitAnimation();
}

void Animation::AddGIF(std::string key)
{
	std::vector<ComPtr<ID2D1Bitmap>> bitmaps = ResourceManager::GetGIFImage(key);

	if (bitmaps.empty()) return;

	for (auto bitmap : bitmaps)
	{
		Frame* frame = new Frame(bitmap);
		frames.push_back(frame);

		if (frames.size() == 1) InitAnimation();
	}
}


ComPtr<ID2D1Bitmap> Animation::InitAnimation()
{
	frameIndex = 0;
	elapsedTime = 0.0f;

	if (frames.empty()) return nullptr;
	else return frames[0]->image;
}
ComPtr<ID2D1Bitmap> Animation::UpdateFrameByTime()
{
	// animator->SetAnimationWhenDone()을 쓸 경우 마지막 프레임 + 마지막 프레임이 된 지 interval 경과일 때 전환되므로 elapsedTime은 갱신을 시켜야 함.
	elapsedTime += Time::GetDeltaTime();
	if (elapsedTime >= interval)
	{
		if (loop == true || IsLastFrame() == false)
		{
			elapsedTime -= interval;
			frameIndex = (frameIndex + 1) % frames.size();
		}
		else
		{
		}
	}

	return frames[frameIndex]->image;
}
ComPtr<ID2D1Bitmap> Animation::NextFrame()
{
	if (loop == true || IsLastFrame() == false)
	{
		frameIndex = (frameIndex + 1) % frames.size();
	}

	return frames[frameIndex]->image;
}
bool Animation::IsImageExist()
{
	return frames.size() > 0;
}
float Animation::GetInterval()
{
	return interval;
}
ComPtr<ID2D1Bitmap> Animation::GetImage(int index)
{
	if (index < frames.size())
	{
		return frames[index]->image;
	}
}
void Animation::SetInterval(float interval)
{
	this->interval = interval;
}
int Animation::GetFrameCount()
{
	return frames.size();
}
int Animation::GetFrameIndex()
{
	return frameIndex;
}
void Animation::SetFrameIndex(int idx)
{
	if (idx >= 0 && idx < frames.size())
	{
		frameIndex = idx;
	}
}
bool Animation::IsLastFrame()
{
	return frameIndex == frames.size() - 1;
}
bool Animation::IsLastFrameAndElapsed()
{
	if (elapsedTime >= interval && IsLastFrame())
	{
		return true;
	}
	return false;
}
Animation::~Animation()
{
	for (int i = 0; i < frames.size(); i++)
	{
		if (frames[i] != nullptr) delete frames[i];
	}
}






void AtlasAnimation::SetAtlasByKey(std::string key)
{
	atlas = ResourceManager::GetAtlasImage(key);
	if (atlas == nullptr) return;

	if (atlas.get()->rects.size() > 0) InitAnimation();
}

std::pair<ComPtr<ID2D1Bitmap>, RectF> AtlasAnimation::InitAnimation()
{
	frameIndex = 0;
	elapsedTime = 0.0f;

	if (atlas.get()->rects.empty()) return std::make_pair(nullptr, RectF{ 0, 0, 0, 0 });
	else return std::make_pair(atlas.get()->image, atlas.get()->rects[0]);
}
std::pair<ComPtr<ID2D1Bitmap>, RectF> AtlasAnimation::UpdateFrameByTime()
{
	// animator->SetAnimationWhenDone()을 쓸 경우 마지막 프레임 + 마지막 프레임이 된 지 interval 경과일 때 전환되므로 elapsedTime은 갱신을 시켜야 함.
	elapsedTime += Time::GetDeltaTime();
	if (elapsedTime >= interval)
	{
		if (loop == true || IsLastFrame() == false)
		{
			elapsedTime -= interval;
			frameIndex = (frameIndex + 1) % atlas.get()->rects.size();
		}
		else
		{
		}
	}

	return std::make_pair(atlas.get()->image, atlas.get()->rects[frameIndex]);
}
std::pair<ComPtr<ID2D1Bitmap>, RectF> AtlasAnimation::NextFrame()
{
	if (loop == true || IsLastFrame() == false)
	{
		frameIndex = (frameIndex + 1) % atlas.get()->rects.size();
	}

	return std::make_pair(atlas.get()->image, atlas.get()->rects[frameIndex]);
}
bool AtlasAnimation::IsImageExist()
{
	return atlas.get()->image != nullptr;
}
float AtlasAnimation::GetInterval()
{
	return interval;
}
ComPtr<ID2D1Bitmap> AtlasAnimation::GetAtlasImage()
{
	return atlas.get()->image;
}
void AtlasAnimation::SetInterval(float interval)
{
	this->interval = interval;
}
int AtlasAnimation::GetFrameCount()
{
	return atlas.get()->rects.size();
}
int AtlasAnimation::GetFrameIndex()
{
	return frameIndex;
}
void AtlasAnimation::SetFrameIndex(int idx)
{
	if (idx >= 0 && idx < atlas.get()->rects.size())
	{
		frameIndex = idx;
	}
}
bool AtlasAnimation::IsLastFrame()
{
	return frameIndex == atlas.get()->rects.size() - 1;
}
bool AtlasAnimation::IsLastFrameAndElapsed()
{
	if (elapsedTime >= interval && IsLastFrame())
	{
		return true;
	}
	return false;
}