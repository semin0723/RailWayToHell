#include "SceneEnterEffecter.h"

void SceneEnterEffecter::Awake()
{
	SetObjectLayer(ObjectLayer::UI);
	SetDrawingPriority(999999);

	shader = AddComponent<SpriteRenderer>();
	shader->SetSize(Camera::GetMainCamera()->width, Camera::GetMainCamera()->height);
	shader->SetImage("shade");

	shader->enable = false;
}

void SceneEnterEffecter::Start()
{
	shader->enable = true;

	AddTimerFunction([=](bool lu)
		{
			shader->color.A -= 1.0f * Time::GetDeltaTime();
			if (lu)
			{
				shader->enable = false;
				this->Kill();
			}
		}, true, 1.0f, 1);
}
