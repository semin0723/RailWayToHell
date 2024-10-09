#include "TurnLeft.h"
#include "GameManager.h"

//void TurnLeft::Awake()
//{
//	SetObjectLayer(ObjectLayer::UI);
//	renderer = AddComponent<TextRenderer>();
//	renderer->SetFont(ResourceManager::GetFont("sample"));
//	renderer->text = std::to_wstring(turn) + L" Turns Left";
//	renderer->color = Color{ 0, 0, 0, 1 };
//	renderer->SetFontSize(30.0f);
//
//	transform->SetPosition(-100.f, 340.f, 0);
//	gameManager = GetOwner()->GetObjectByName<GameManager>("GameManager");
//}
//
//void TurnLeft::Update()
//{
//	if (gameManager->GetLeftTurn() > 0)
//	{
//		renderer->text = std::to_wstring(gameManager->GetLeftTurn()) + L" Turns Left";
//		renderer->enable = true;
//	}
//	else
//	{
//		renderer->enable = false;
//	}
//}