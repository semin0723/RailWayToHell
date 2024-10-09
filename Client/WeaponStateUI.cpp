#include "WeaponStateUI.h"
#include "Player.h"

void WeaponStateUI::Awake()
{
	renderer = AddComponent<SpriteRenderer>();

	// UI/WeaponAttack,  UI/WeaponKnockBack
	renderer->SetImage(ResourceManager::GetImage("UI/WeaponAttack"));
	renderer->SetSize(80, 80);
	renderer->enable = false;
}

void WeaponStateUI::Start()
{
	player = GetOwner()->GetObjectByName<Player>("Player");
}

void WeaponStateUI::ChangeWeaponState(std::string state)
{
	// 추후 상태 변수가 추가될 때 변경 필수
	if (state == "Attack") {
		renderer->SetImage(ResourceManager::GetImage("UI/WeaponAttack"));
	}
	else if (state == "KnockBack") {
		renderer->SetImage(ResourceManager::GetImage("UI/WeaponKnockBack"));
	}
}
