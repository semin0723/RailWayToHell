#include "Dialog.h"
#include "TextObject.h"

void Dialog::Awake()
{
	spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetImage(ResourceManager::GetImage("UI/TextBox"));



	if (ObjectLayer::Object == GetObjectLayer())
	{
		textObject = AddObject<TextObject>();
		SetDrawingPriority(OrderLayer::dialog);
		textObject->SetDrawingPriority(OrderLayer::text);
		textObject->GetComponent<TextRenderer>()->SetFont(ResourceManager::GetFont("Font/Script"));
		textObject->GetComponent<TextRenderer>();
	}
	else
	{
		textObject = AddUIObject<TextObject>();
		textObject->GetComponent<TextRenderer>()->SetFont(ResourceManager::GetFont("Font/UI"));
	}

	//textObject->GetComponent<TextRenderer>()->SetAlignment(TextAlignHorizontal::Center);
	textObject->GetComponent<TextRenderer>()->fontSize = 20.0f;
	textObject->transform->SetParent(transform);
	
	
}

void Dialog::Start()
{
}

void Dialog::Update()
{
	if (transform->GetParent())
	{
		transform->SetScaleX(transform->GetParent()->GetScale().x > 0.0f ? 1.0f : -1.0f);
	}
}

void Dialog::SetText(std::wstring text, int fontSize)
{
	std::wstringstream stream(text);

	int newlineCount = std::count(text.begin(), text.end(), '\n');

	textObject->GetComponent<TextRenderer>()->text = text;
	textObject->GetComponent<TextRenderer>()->fontSize = fontSize;
	textObject->transform->SetPositionX(text.size() * 0.45f * -fontSize);
	textObject->transform->Translate(fontSize * 1.1f, (newlineCount + 1) * fontSize * 0.7f, 0.0f);

	spriteRenderer->SetSize(text.size() * fontSize, (newlineCount + 1) * 100.0f);
}

void Dialog::SetText(std::wstring text, int fontSize, Vector3 offset)
{
	std::wstringstream stream(text);

	int newlineCount = std::count(text.begin(), text.end(), '\n');

	textObject->GetComponent<TextRenderer>()->text = text;
	textObject->GetComponent<TextRenderer>()->fontSize = fontSize;
	textObject->transform->SetPositionX(text.size() * 0.45f * -fontSize);
	textObject->transform->Translate(fontSize * 1.1f, (newlineCount + 1) * fontSize * 0.7f, 0.0f);
	textObject->transform->Translate(offset);

	spriteRenderer->SetSize(text.size() * fontSize, (newlineCount + 1) * 100.0f);
}
