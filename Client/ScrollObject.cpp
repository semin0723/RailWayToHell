#include "ScrollObject.h"
#include <string>
#include "Config.h"



void ScrollObject::Awake()
{
	{
		sprite1 = AddObject<Object>();
		SpriteRenderer* sprite = sprite1->AddComponent<SpriteRenderer>();

		sprite->SetSize(40000.0f, Camera::GetMainCamera()->height + 20);
		sprite->spriteMode = SpriteMode::Repeat;
		sprite1->SetDrawingPriority(OrderLayer::BackGround);
		sprite1->transform->SetParent(transform);
		sprite1->SetDontDestroyOnSceneChange(true);
	}
	{
		sprite2 = AddObject<Object>();
		SpriteRenderer* sprite = sprite2->AddComponent<SpriteRenderer>();

		sprite->SetSize(40000.0f, Camera::GetMainCamera()->height + 20);
		sprite->spriteMode = SpriteMode::Repeat;
		sprite2->SetDrawingPriority(OrderLayer::BackGround);
		sprite2->transform->SetParent(transform);
		sprite2->SetDontDestroyOnSceneChange(true);
	}

	// юс╫ц...
	/*Object* nineSlicedSample = AddUIObject<Object>();
	nineSlicedSample->SetDrawingPriority(100000);
	SpriteRenderer* render = nineSlicedSample->AddComponent<SpriteRenderer>();
	render->SetSize(250, 250);
	render->useNineSliced = true;
	render->nineSlicedImage = ResourceManager::GetNineSlicedImage("sample");
	float t = 0.0f;
	nineSlicedSample->AddTimerFunction([=](bool lu) mutable
		{
			t += Time::GetDeltaTime() / 5.f;
			render->SetSize(Math::Lerp(250, 500, t), Math::Lerp(250, 500, t));
		}, true, 5.f, 1);*/
}

void ScrollObject::Update()
{
	transform->SetPositionX(transform->GetPosition().x - speed * Time::GetDeltaTime());
	if (transform->GetPosition().x < -20000.0f + 1920.f)
	{

		transform->Translate(20000.0f, 0.0f, 0.0f);
	}


}

void ScrollObject::ImageChange(std::string imageName)
{
	if (this->imageName != imageName)
	{
		sprite2->GetComponent<SpriteRenderer>()->SetImage(imageName);
		sprite2->SetEnable(true);
		this->imageName = imageName;

		SpriteRenderer* sprite = sprite2->GetComponent<SpriteRenderer>();
		sprite->color.A = 0.0f;
		float t = 0.0f;

		AddTimerFunction([sprite, t](bool lu) mutable
						 {

							 if (!lu)
							 {
								 t += Time::GetDeltaTime() / GetConfig().BackGround_ChageTime;
								 sprite->color.A = t;
							 }
							 else
							 {
								 sprite->color.A = 1.0f;
							 }

						 }, true, GetConfig().BackGround_ChageTime, 1);


		SpriteRenderer* sprite_2 = sprite1->GetComponent<SpriteRenderer>();

		AddTimerFunction([this, sprite_2, t](bool lu) mutable
						 {

							 if (!lu)
							 {
								 t += Time::GetDeltaTime() / GetConfig().BackGround_ChageTime;
								 sprite_2->color.A = 1.0f - t;
							 }
							 else
							 {
								 sprite_2->color.A = 0.0f;

							 }

						 }, true, GetConfig().BackGround_ChageTime, 1);

		std::swap(sprite1, sprite2);
	}
}

using namespace std::string_literals;

void ScrollBackGround::Awake()
{
	BackGround5 = AddObject<ScrollObject>();
	BackGround4 = AddObject<ScrollObject>();
	BackGround3 = AddObject<ScrollObject>();
	BackGround2 = AddObject<ScrollObject>();
	BackGround1 = AddObject<ScrollObject>();

	SetDontDestroyOnSceneChange(true);
	BackGround1->SetDontDestroyOnSceneChange(true);
	BackGround2->SetDontDestroyOnSceneChange(true);
	BackGround3->SetDontDestroyOnSceneChange(true);
	BackGround4->SetDontDestroyOnSceneChange(true);
	BackGround5->SetDontDestroyOnSceneChange(true);
}

void ScrollBackGround::SetBackGround(std::string backGroundName)
{
	std::string back1 = "BackGround_"s + backGroundName + "_1";
	std::string back2 = "BackGround_"s + backGroundName + "_2";
	std::string back3 = "BackGround_"s + backGroundName + "_3";
	std::string back4 = "BackGround_"s + backGroundName + "_4";
	std::string back5 = "BackGround_"s + backGroundName + "_5";

	BackGround5->speed = 500.0f;
	BackGround5->ImageChange(back5);

	BackGround4->speed = 600.0f;
	BackGround4->ImageChange(back4);
	
	BackGround3->speed = 700.0f;
	BackGround3->ImageChange(back3);
	
	BackGround2->speed = 800.0f;
	BackGround2->ImageChange(back2);

	BackGround1->speed = 900.0f;
	BackGround1->ImageChange(back1);


	printf("\n");
}
