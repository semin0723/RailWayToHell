#pragma once
#include "Engine.h"
#include "Dialog.h"

class ChatBalloonComponent : public Script
{
public:
	ChatBalloonComponent() {}
	~ChatBalloonComponent() {}

	virtual void Update() override;

	void BubbleDialog(std::wstring text, float fontSize, float bubbleDuration, bool isOverlay = false, Vector3 offset = Vector3(0, 0, 0));
	void BubbleDialog(std::string text, float fontSize, float bubbleDuration, bool isOverlay = false, Vector3 offset = Vector3(0, 0, 0));



private:
	Dialog* curDialog = nullptr;
	float duration = 0;

};

