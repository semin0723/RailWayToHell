#include "ChatBalloonComponent.h"

void ChatBalloonComponent::Update()
{
	if (curDialog == nullptr) return;

	duration -= Time::GetDeltaTime();

	if (duration <= 0) {
		duration = 0;
		curDialog->Kill();
		curDialog = nullptr;
	}
}

void ChatBalloonComponent::BubbleDialog(std::wstring text, float fontSize, float bubbleDuration, bool isOverlay, Vector3 offset)
{
	if (curDialog)
	{
		if (!isOverlay)
		{
			return;
		}
		duration = 0;
		curDialog->Kill();
		curDialog = nullptr;
	}
	curDialog = GetOwner()->AddObject<Dialog>();
	curDialog->SetDrawingPriority(OrderLayer::dialog);
	curDialog->SetText(text, fontSize, offset);


	curDialog->transform->SetParent(GetOwner()->transform);
	curDialog->transform->SetPositionY(GetOwner()->GetComponent<SpriteRenderer>()->GetHeight() + curDialog->GetComponent<SpriteRenderer>()->GetHeight());
	duration = bubbleDuration;
}

void ChatBalloonComponent::BubbleDialog(std::string text, float fontSize, float bubbleDuration, bool isOverlay, Vector3 offset)
{
    wchar_t tempstr[256];
    // Convert the multibyte string to a wide string
	MultiByteToWideChar(CP_OEMCP, 0, text.c_str(), -1, tempstr, 256);

	BubbleDialog(tempstr, fontSize, bubbleDuration, isOverlay, offset);
}
