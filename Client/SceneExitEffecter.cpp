#include "SceneExitEffecter.h"

void SceneExitEffecter::Awake()
{
	SetObjectLayer(ObjectLayer::UI);
	SetDrawingPriority(999999);

	shader = AddComponent<SpriteRenderer>();
	shader->SetSize(Camera::GetMainCamera()->width, Camera::GetMainCamera()->height);
	shader->SetImage("shade");

	shader->enable = false;
}
