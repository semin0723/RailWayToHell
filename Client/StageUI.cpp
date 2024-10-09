#include "StageUI.h"

void StageUI::Awake()
{
	transform->SetPosition(0, 400.f, 0);

	//Object* leftTop = AddObject<Object>();
	//SpriteRenderer* ltRender = leftTop->AddComponent<SpriteRenderer>();
	//ltRender->color.A = 0.4f;
	//ltRender->SetImage("UI/BackUI");

	//Object* rightTop = AddObject<Object>();
	//SpriteRenderer* rtRender = rightTop->AddComponent<SpriteRenderer>();
	//rtRender->color.A = 0.4f;
	//rtRender->SetImage("UI/BackUI");

	//ltRender->SetSize(440, 200);
	//leftTop->transform->SetPosition(-700, 0, 0);
	//leftTop->transform->SetParent(transform);

	//rtRender->SetSize(440, 200);
	//rightTop->transform->SetPosition(700, 0, 0);
	//rightTop->transform->SetParent(transform);
}
