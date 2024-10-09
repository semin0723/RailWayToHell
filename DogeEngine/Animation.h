#pragma once
#include <vector>
#include <string>
#include <memory>
#include "DirectXFramework.h"
#include "Vector2.h"
#include "Structures.h"
#include "defines.h"
#include "AtlasImage.h"

class SpriteRenderer;
class Animator;

NAMESPACE_DOGEENGINE_BEGIN
struct Frame
{
	ComPtr<ID2D1Bitmap> image{ nullptr };
	Vector2 anchor{ 0, 0 };
	Frame(ComPtr<ID2D1Bitmap> image)
	{
		this->image = image;
	}
	Frame(ComPtr<ID2D1Bitmap> image, Vector2 anchor)
	{
		this->image = image;
		this->anchor = anchor;
	}
};

class AnimationBase
{
public:
	virtual bool IsImageExist() = 0;
	virtual float GetInterval() = 0;
	virtual void SetInterval(float interval) = 0;
	virtual int GetFrameCount() = 0;
	virtual int GetFrameIndex() = 0;
	virtual void SetFrameIndex(int index) = 0;
	virtual bool IsLastFrame() = 0;
	virtual bool IsLastFrameAndElapsed() = 0;

	virtual ~AnimationBase() {}
};
NAMESPACE_DOGEENGINE_END



class Animation : public DG::AnimationBase
{
	std::vector<DG::Frame*> frames;
	float elapsedTime = 0;
	int frameIndex = 0;
	float interval = 0.05f;
public:
	bool loop = true;

	void AddFrame(std::string imageKey);
	void AddGIF(std::string gifKey);

	ComPtr<ID2D1Bitmap> InitAnimation();
	ComPtr<ID2D1Bitmap> UpdateFrameByTime();
	ComPtr<ID2D1Bitmap> NextFrame();
	ComPtr<ID2D1Bitmap> GetImage(int index);

	bool IsImageExist() override;
	float GetInterval() override;
	void SetInterval(float interval) override;
	int GetFrameCount() override;
	int GetFrameIndex() override;
	void SetFrameIndex(int idx) override;
	bool IsLastFrame() override;
	bool IsLastFrameAndElapsed() override;

	~Animation();
};



class AtlasAnimation : public DG::AnimationBase
{
	std::shared_ptr<AtlasImage> atlas;
	float elapsedTime = 0.0f;
	int frameIndex = 0;
	float interval = 0.05f;
public:
	bool loop = true;

	void SetAtlasByKey(std::string key);

	std::pair<ComPtr<ID2D1Bitmap>, RectF> InitAnimation();
	std::pair<ComPtr<ID2D1Bitmap>, RectF> UpdateFrameByTime();
	std::pair<ComPtr<ID2D1Bitmap>, RectF> NextFrame();
	ComPtr<ID2D1Bitmap> GetAtlasImage();

	bool IsImageExist() override;
	float GetInterval() override;
	void SetInterval(float interval) override;
	int GetFrameCount() override;
	int GetFrameIndex() override;
	void SetFrameIndex(int idx) override;
	bool IsLastFrame() override;
	bool IsLastFrameAndElapsed() override;
};