#include "Cursor_Aim.h"
#include "Config.h"

void Cursor_Aim::Awake()
{
	int size = GetConfig().UI_CursorSize;

	renderer = AddComponent<SpriteRenderer>();
	renderer->SetSize(size, size);

	SetObjectLayer(ObjectLayer::UI);
	SetDrawingPriority(99999);

	clientRect = Camera::GetMainCamera()->GetScreenRect();
}

void Cursor_Aim::Start()
{
	SetGunCursor();
}

void Cursor_Aim::Update()
{
	Vector3 pos = Input::GetMousePos();
	pos.y = -pos.y;
	pos.x -= clientRect.right / 2.f;
	pos.y += clientRect.bottom / 2.f;
	transform->SetPosition(pos);
}
void Cursor_Aim::SetGunCursor()
{
	renderer->SetImage("UI/GunAim");
}
void Cursor_Aim::SetCrossbowCursor()
{
	renderer->SetImage("UI/CrossBowAim");
}
