#include "PrintActionPriority.h"
#include "GameObject.h"

void PrintActionPriority::Awake()
{
	textObj = GetOwner()->AddObject<Object>();
	textObj->transform->SetParent(GetOwner()->transform);
	textObj->SetDrawingPriority(OrderLayer::indicator_default);
	//textObj->transform->SetParent(GetOwner()->transform);

	renderer = textObj->AddComponent<SpriteRenderer>();
	renderer->SetSize(30, 30);

	textObj->SetEnable(false);
}

void PrintActionPriority::Start()
{
	float height = GetOwner()->GetComponent<SpriteRenderer>()->GetHeight();
	textObj->transform->SetPositionY(height + 25.0f);
}

void PrintActionPriority::PrintPriority(int priority)
{
	textObj->transform->SetScaleX(GetOwner()->transform->GetScale().x);

	textObj->SetEnable(true);

	std::string key = "UI/ActionPriority/" + std::to_string(priority);
	renderer->SetImage(key);
}

void PrintActionPriority::HidePriority()
{
	textObj->SetEnable(false);
}

PrintActionPriority::~PrintActionPriority()
{
}
