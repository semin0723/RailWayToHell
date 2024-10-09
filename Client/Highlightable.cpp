#include "Highlightable.h"
#include "Player.h"
#include "GameObject.h"
#include "defines.h"
#include "MonsterComponent.h"
#include "GameManager.h"
#include "HealthComponent.h"
#include "Trap.h"
#include "TacticModeBtn.h"
#include "Boss.h"
#include "BossLeftHand.h"
#include "BossRightHand.h"

void HighlightManager::Start()
{
	gameManager = GetOwner()->GetObjectByName<GameManager>("GameManager");
	player = GetOwner()->GetObjectByName<Player>("Player");
	tacticBtn = GetOwner()->GetObjectByName<TacticModeBtn>("TacticModeBtn");
}

void HighlightManager::Update()
{

}

void HighlightManager::FixedUpdate()
{
	//UpdateIndicatorShape();
	UpdateHighlight();
}

void HighlightManager::UpdateHighlight()
{
	bool isTactic = tacticBtn->on == true ? true : false;
	PlayerPhase phase = player->phase;
	WeaponType type = player->currentWeapon;

	// isHover = false로 초기화
	GameObject* hoveredTarget = UpdateHighlightStateAndFindHoveredTarget();
	if (hoveredTarget != nullptr)
	{
		if (gameManager->GetCurrentPhase() == Phase::Type::Player_Action)
		{
			if (hoveredTarget != lastHoveredObject)
			{
				AudioFramework::Play_BankSound("Sound", "UI_Mouseover");
				lastHoveredObject = hoveredTarget;
			}
		}
		else
		{
			lastHoveredObject = nullptr;
		}
	}

	// hoveredTarget에 대해 isHover = true로 덮어쓰기
	// 화약통, 샹들리에의 경우 isHover = true로 덮어쓸 때 주변 오브젝트에 대한 처리까지 수행함.
	if (hoveredTarget != nullptr)
	{
		hoveredTarget->GetComponent<Highlightable>()->ChangeState(isTactic, true, phase, type);
	}

	if (tutorial_target)
	{
		if (tutorial_target->type == EType::Enemy) tutorial_target->GetComponent<Highlightable_Monster>()->highlight_sprite = true;
		else if (tutorial_target->type == EType::Structure)
		{
			Highlightable_Chandelier* chand = tutorial_target->GetComponent<Highlightable_Chandelier>();
			if (chand != nullptr) chand->highlight_sprite = true;

			Highlightable_Gunpowder* gp = tutorial_target->GetComponent<Highlightable_Gunpowder>();
			if (gp != nullptr) gp->highlight_sprite = true;
		}
	}
}

GameObject* HighlightManager::UpdateHighlightStateAndFindHoveredTarget()
{
	GameObject* hoveredTarget = nullptr;
	std::vector<TileObject*>& tileObjects = gameManager->tileObjects;

	bool isTactic = tacticBtn->on == true ? true : false;
	PlayerPhase phase = player->phase;
	WeaponType type = player->currentWeapon;

	// 캐릭터가 화약통보다 앞에 있으므로 두 콜라이더가 겹쳤을 때를 대비하여 캐릭터를 나중에 체크한다.
	for (auto tObj : tileObjects)
	{
		if (tObj->chandelier != nullptr && tObj->chandelier->GetStarted() == true)
		{
			tObj->chandelier->GetComponent<Highlightable>()->ChangeState(isTactic, false, phase, type);
			if (tObj->chandelier->GetComponent<Collider>()->IsCollideWithCursor())
			{
				hoveredTarget = tObj->chandelier;
			}
		}
		if (tObj->gunPowder != nullptr && tObj->gunPowder->GetStarted() == true)
		{
			tObj->gunPowder->GetComponent<Highlightable>()->ChangeState(isTactic, false, phase, type);
			if (tObj->gunPowder->GetComponent<Collider>()->IsCollideWithCursor())
			{
				hoveredTarget = tObj->gunPowder;
			}
		}

		if (tObj->object != nullptr && tObj->object->type == EType::Player && tObj->object->GetStarted() == true)
		{
 			tObj->object->GetComponent<Highlightable>()->ChangeState(isTactic, false, phase, type);
		}
		else if (tObj->object != nullptr && tObj->object->type == EType::Enemy && tObj->object->GetStarted() == true)
		{
			tObj->object->GetComponent<Highlightable>()->ChangeState(isTactic, false, phase, type);
			if (tObj->object->GetComponent<Collider>()->IsCollideWithCursor())
			{
				hoveredTarget = tObj->object;
			}
		}
	}

	if (gameManager->bossMonster != nullptr)
	{
		if (gameManager->bossMonster != nullptr && gameManager->bossMonster->GetStarted() == true)
		{
			gameManager->bossMonster->GetComponent<Highlightable>()->ChangeState(isTactic, false, phase, type);
			if (gameManager->bossMonster->IsHandAlive() == false && gameManager->bossMonster->GetComponent<Collider>()->IsCollideWithCursor())
			{
				hoveredTarget = gameManager->bossMonster;
			}
		}
		if (gameManager->bossMonster->rightHand != nullptr && gameManager->bossMonster->rightHand->GetStarted() == true)
		{
			gameManager->bossMonster->rightHand->GetComponent<Highlightable>()->ChangeState(isTactic, false, phase, type);
			if (gameManager->bossMonster->rightHand->GetComponent<Collider>()->IsCollideWithCursor())
			{
				hoveredTarget = gameManager->bossMonster->rightHand;
			}
		}
		if (gameManager->bossMonster->leftHand != nullptr && gameManager->bossMonster->leftHand->GetStarted() == true)
		{
			gameManager->bossMonster->leftHand->GetComponent<Highlightable>()->ChangeState(isTactic, false, phase, type);
			if (gameManager->bossMonster->leftHand->GetComponent<Collider>()->IsCollideWithCursor())
			{
				hoveredTarget = gameManager->bossMonster->leftHand;
			}
		}
	}

	return hoveredTarget;
}

void HighlightManager::UpdateIndicatorShape()
{
	std::vector<TileObject*>& tileObjects = gameManager->tileObjects;

 	for (auto tObj : tileObjects)
	{
		if (tObj->chandelier != nullptr)
		{
			// foo~
		}
		if (tObj->gunPowder != nullptr)
		{
			// foo~
		}

		if (tObj->object != nullptr && tObj->object->type == EType::Player)
		{
			// foo~
		}
		else if (tObj->object != nullptr && tObj->object->type == EType::Enemy && tObj->object->GetStarted() == true)
		{
			if (tObj->object->GetComponent<MonsterComponent>()->IsBossMonster())
			{
				continue;
			}

			MonsterComponent* mobComp = tObj->object->GetComponent<MonsterComponent>();
			Highlightable_Monster* highlight = tObj->object->GetComponent<Highlightable_Monster>();
			// 모든 화살표 UI의 사이즈는 원본 사이즈 그대로임.
			//float scale = (float)index_distance_between_mob_And_player;
			int act_direction = mobComp->GetActDirection();

			int damage = mobComp->GetDamage();
			std::string key = "Damage/" + std::to_string(damage);
			highlight->action_and_damage_renderer->SetImage(key.c_str());

			// attack_arrow는 원거리 적만 사용한다.
			int attack_range = mobComp->GetAttackIDistance();
			if (mobComp->GetEnemyType() == EnemyType::Far && mobComp->GetActType() == ActType::Attack)
			{
				if (attack_range == 1)
				{
					highlight->attack_arrow_range_renderer->SetImage("arrow/arc1");
					highlight->attack_arrow_range_renderer->SetSizeAuto();
				}
				else if (attack_range == 2)
				{
					highlight->attack_arrow_range_renderer->SetImage("arrow/arc2");
					highlight->attack_arrow_range_renderer->SetSizeAuto();
				}
			}

			SetArrowScaleAndAnchorByFaceDirection(tObj->object, highlight->attack_arrow_range_renderer, act_direction, 1.0f);
			SetArrowScaleAndAnchorByFaceDirection(tObj->object, highlight->attack_arrow_melee_renderer, act_direction, 1.0f);
			SetArrowScaleAndAnchorByFaceDirection(tObj->object, highlight->move_arrow_renderer, act_direction, 1.0f);

			SetAnotherIndicatorScaleByFaceDirection(tObj->object, highlight->action_and_damage_renderer);
			SetAnotherIndicatorScaleByFaceDirection(tObj->object, highlight->tile_select_renderer);
		}
	}
}

void HighlightManager::SetArrowScaleAndAnchorByFaceDirection(GameObject* subject, SpriteRenderer* arrow_renderer, int act_dir, float length)
{
	if (act_dir == 1)
	{
		if (subject->transform->GetScale().x > 0)
		{
			arrow_renderer->GetOwner()->transform->SetScaleX(length);
			arrow_renderer->anchorType = AnchorType::LeftCenter;
		}
		else if (subject->transform->GetScale().x < 0)
		{
			arrow_renderer->GetOwner()->transform->SetScaleX(-length);
			arrow_renderer->anchorType = AnchorType::LeftCenter;
		}
	}
	else if (act_dir == -1)
	{
		if (subject->transform->GetScale().x > 0)
		{
			arrow_renderer->GetOwner()->transform->SetScaleX(-length);
			arrow_renderer->anchorType = AnchorType::LeftCenter;
		}
		else if (subject->transform->GetScale().x < 0)
		{
			arrow_renderer->GetOwner()->transform->SetScaleX(length);
			arrow_renderer->anchorType = AnchorType::LeftCenter;
		}
	}
}

void HighlightManager::SetAnotherIndicatorScaleByFaceDirection(Object* subject, SpriteRenderer* indicator_renderer)
{
	if (subject->transform->GetScale().x > 0)
	{
		indicator_renderer->GetOwner()->transform->SetScaleX(1.f);
	}
	else if (subject->transform->GetScale().x < 0)
	{
		indicator_renderer->GetOwner()->transform->SetScaleX(-1.f);
	}
}

void Highlightable::ChangeColorSoftly(bool showing, SpriteRenderer* targetRenderer, float duration, bool change_r, bool change_g, bool change_b, bool change_a, float value_max, float value_min)
{
	if (showing == true)
	{
		if (targetRenderer->enable == false) targetRenderer->enable = true;

		if (change_r)
		{
			targetRenderer->color.R += Time::GetRealDeltaTime() / duration;
			if (targetRenderer->color.R > value_max) targetRenderer->color.R = value_max;
		}
		if (change_g)
		{
			targetRenderer->color.G += Time::GetRealDeltaTime() / duration;
			if (targetRenderer->color.G > value_max) targetRenderer->color.G = value_max;
		}
		if (change_b)
		{
			targetRenderer->color.B += Time::GetRealDeltaTime() / duration;
			if (targetRenderer->color.B > value_max) targetRenderer->color.B = value_max;
		}
		if (change_a)
		{
			targetRenderer->color.A += Time::GetRealDeltaTime() / duration;
			if (targetRenderer->color.A > value_max) targetRenderer->color.A = value_max;
		}
	}
	else if (showing == false)
	{
		if (change_r)
		{
			targetRenderer->color.R -= Time::GetRealDeltaTime() / duration;
			if (targetRenderer->color.R < value_min)
			{
				targetRenderer->color.R = value_min;
				if (value_min == 0.0f) targetRenderer->enable = false;
			}
		}
		if (change_g)
		{
			targetRenderer->color.G -= Time::GetRealDeltaTime() / duration;
			if (targetRenderer->color.G < value_min)
			{
				targetRenderer->color.G = value_min;
				if (value_min == 0.0f) targetRenderer->enable = false;
			}
		}
		if (change_b)
		{
			targetRenderer->color.B -= Time::GetRealDeltaTime() / duration;
			if (targetRenderer->color.B < value_min)
			{
				targetRenderer->color.B = value_min;
				if (value_min == 0.0f) targetRenderer->enable = false;
			}
		}
		if (change_a)
		{
			targetRenderer->color.A -= Time::GetRealDeltaTime() / duration;
			if (targetRenderer->color.A < value_min)
			{
				targetRenderer->color.A = value_min;
				if (value_min == 0.0f) targetRenderer->enable = false;
			}
		}
	}
}

void Highlightable::ChangeState(bool isTacticMode, bool isHover, PlayerPhase playerPhase, WeaponType weaponType)
{
	if (GetOwner()->GetStarted() == false) return;

	if (gameManager->GetCurrentPhase() != Phase::Player_Action)
	{
		if (isHover)
		{
			OnAnotherPhase_Hover();
		}
		else
		{
			OnAnotherPhase();
		}
		return;
	}

	if (isTacticMode == true)
	{
		if (playerPhase != PlayerPhase::Aim)
		{
			if (isHover)
			{
				OnTacticMode_Idle_Hover();
			}
			else
			{
				OnTacticMode_Idle();
			}
		}
		else
		{
			if (weaponType == WeaponType::Gun)
			{
				if (isHover)
				{
					OnTacticMode_Aim_Gun_Hover();
				}
				else
				{
					OnTacticMode_Aim_Gun();
				}
			}
			else if (weaponType == WeaponType::Crossbow)
			{
				if (isHover)
				{
					OnTacticMode_Aim_Crossbow_Hover();
				}
				else
				{
					OnTacticMode_Aim_Crossbow();
				}
			}
		}
	}
	else if (isTacticMode == false)
	{
		if (playerPhase != PlayerPhase::Aim)
		{
			if (isHover)
			{
				OnNormalMode_Idle_Hover();
			}
			else
			{
				OnNormalMode_Idle();
			}
		}
		else
		{
			if (weaponType == WeaponType::Gun)
			{
				if (isHover)
				{
					OnNormalMode_Aim_Gun_Hover();
				}
				else
				{
					OnNormalMode_Aim_Gun();
				}
			}
			else if (weaponType == WeaponType::Crossbow)
			{
				if (isHover)
				{
					OnNormalMode_Aim_Crossbow_Hover();
				}
				else
				{
					OnNormalMode_Aim_Crossbow();
				}
			}
		}
	}
}

void Highlightable::UpdateFlickering(bool isFlickering, class HealthComponent* healthComp, int decresed_hp)
{
	if (healthComp == nullptr) return;

	if (isFlickering)
	{
		healthComp->TurnOnFlickering(decresed_hp);
	}
	else
	{
		healthComp->TurnOffFlickering();
	}
}

// 플레이어 체력 깜빡거리는 건 하지 말자
void Highlightable::IsPlayerAttacked()
{
	/*std::vector<TileObject*> tileObjects = gameManager->tileObjects;
	
	if (player->GetCurrentTileIndex() != player->moveTargetTileIndex)
	{
		if (tileObjects[player->moveTargetTileIndex]->object != nullptr)
		{
			std::swap(tileObjects[player->moveTargetTileIndex], tileObjects[player->GetCurrentTileIndex()]);
		}
	}

	*/
}

void Highlightable::Start()
{
	player = GetOwner()->GetOwner()->GetObjectByName<Player>("Player");
	gameManager = GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager");
	type = TargetType::Player;
}






void Highlightable_Player::Awake()
{
}

void Highlightable_Player::Start()
{
	player = GetOwner()->GetOwner()->GetObjectByName<Player>("Player");
	gameManager = GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager");
 	type = TargetType::Player;

	sprite_renderer = GetOwner()->GetComponent<SpriteRenderer>();

	healthComp = GetOwner()->GetComponent<HealthComponent>();

	Object* obj;

	obj = GetOwner()->AddObject<Object>();
	obj->SetDrawingPriority(OrderLayer::indicator_default);
	obj->transform->SetPositionY(sprite_renderer->GetHeight() * 0.5f);
	obj->transform->SetParent(GetOwner()->transform);
	knockback_arrow_renderer = obj->AddComponent<SpriteRenderer>();
	knockback_arrow_renderer->SetImage("arrow/knockback");
	knockback_arrow_renderer->SetSizeAuto();
	knockback_arrow_renderer->color.A = 0.0f;
	knockback_arrow_renderer->enable = false;


	//GetOwner()->SetChildsDrawingPriority(OrderLayer::Player);
}

void Highlightable_Player::Update()
{
}

void Highlightable_Player::LateUpdate()
{
	ChangeColorSoftly(highlight_sprite, sprite_renderer, highlight_sprite_duration, true, true, true, false, 1.0f, highlight_sprite_min_color);
	ChangeColorSoftly(highlight_knockback_arrow, knockback_arrow_renderer, highlight_knockback_arrow_duration, false, false, false, true, 1.0f, 0.0f);
	UpdateFlickering(flicker_hp, healthComp, decreased_hp_amount_on_flickering);
}

void Highlightable_Player::OnAnotherPhase()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnAnotherPhase_Hover()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnTacticMode_Idle()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnTacticMode_Idle_Hover()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnTacticMode_Aim_Gun()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnTacticMode_Aim_Gun_Hover()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnTacticMode_Aim_Crossbow()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnTacticMode_Aim_Crossbow_Hover()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnNormalMode_Idle()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnNormalMode_Idle_Hover()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnNormalMode_Aim_Gun()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnNormalMode_Aim_Gun_Hover()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnNormalMode_Aim_Crossbow()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Player::OnNormalMode_Aim_Crossbow_Hover()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	flicker_hp = false;
}

void Highlightable_Monster::Awake()
{
}

void Highlightable_Monster::Start()
{
	player = GetOwner()->GetOwner()->GetObjectByName<Player>("Player");
	gameManager = GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager");
	type = TargetType::Enemy;

	sprite_renderer = GetOwner()->GetComponent<SpriteRenderer>();
	mobComp = GetOwner()->GetComponent<MonsterComponent>();

	healthComp = GetOwner()->GetComponent<HealthComponent>();

	Object* obj;

	obj = GetOwner()->AddObject<Object>();
	obj->SetDrawingPriority(OrderLayer::indicator_default);
	obj->transform->SetParent(GetOwner()->transform);
	obj->transform->SetPositionY(0.0f);
	move_arrow_renderer = obj->AddComponent<SpriteRenderer>();
	move_arrow_renderer->SetImage("arrow/attack/melee");
	move_arrow_renderer->SetSizeAuto();
	move_arrow_renderer->color.A = 0.0f;
	move_arrow_renderer->enable = false;

	obj = GetOwner()->AddObject<Object>();
	obj->SetDrawingPriority(OrderLayer::indicator_default);
	obj->transform->SetParent(GetOwner()->transform);
	obj->transform->SetPositionY(sprite_renderer->GetHeight() * 1.3f);
	attack_arrow_range_renderer = obj->AddComponent<SpriteRenderer>();
	attack_arrow_range_renderer->SetImage("arrow/arc1");
	attack_arrow_range_renderer->SetSizeAuto();
	attack_arrow_range_renderer->color.A = 0.0f;
	attack_arrow_range_renderer->enable = false;

	obj = GetOwner()->AddObject<Object>();
	obj->SetDrawingPriority(OrderLayer::indicator_default);
	obj->transform->SetParent(GetOwner()->transform);
	obj->transform->SetPositionY(0.0f);
	attack_arrow_melee_renderer = obj->AddComponent<SpriteRenderer>();
	attack_arrow_melee_renderer->SetImage("arrow/attack/melee");
	attack_arrow_melee_renderer->SetSizeAuto();
	attack_arrow_melee_renderer->color.A = 0.0f;
	attack_arrow_melee_renderer->enable = false;

	obj = GetOwner()->AddObject<Object>();
	obj->SetDrawingPriority(OrderLayer::indicator_default);
	obj->transform->SetParent(GetOwner()->transform);
	obj->transform->SetPositionY(sprite_renderer->GetHeight() * 0.5f);
	knockback_arrow_renderer = obj->AddComponent<SpriteRenderer>();
	knockback_arrow_renderer->SetImage("arrow/knockback");
	knockback_arrow_renderer->SetSizeAuto();
	knockback_arrow_renderer->color.A = 0.0f;
	knockback_arrow_renderer->enable = false;

	obj = GetOwner()->AddObject<Object>();
	obj->SetDrawingPriority(OrderLayer::indicator_default);
	obj->transform->SetParent(GetOwner()->transform);
	obj->transform->SetPositionY(sprite_renderer->GetHeight() + 50);
	action_and_damage_renderer = obj->AddComponent<SpriteRenderer>();
	action_and_damage_renderer->SetImage("Damage/1");
	action_and_damage_renderer->SetSize(50, 50);
	action_and_damage_renderer->color.A = 0.0f;
	action_and_damage_renderer->enable = false;

	obj = GetOwner()->AddObject<Object>();
	obj->SetDrawingPriority(OrderLayer::indicator_default);
	obj->transform->SetParent(GetOwner()->transform);
	obj->transform->SetPositionY(-20.f);
	tile_select_renderer = obj->AddComponent<SpriteRenderer>();
	tile_select_renderer->SetImage("UI/Selected");
	tile_select_renderer->SetSizeAuto();
	tile_select_renderer->color.A = 0.0f;
	tile_select_renderer->enable = false;

	//GetOwner()->SetChildsDrawingPriority(OrderLayer::Enemy);
}

void Highlightable_Monster::Update()
{
}

void Highlightable_Monster::LateUpdate()
{
	ChangeColorSoftly(highlight_sprite, sprite_renderer, highlight_sprite_duration, true, true, true, false, 1.0f, highlight_sprite_min_color);
	ChangeColorSoftly(highlight_move_arrow, move_arrow_renderer, highlight_move_arrow_duration, false, false, false, true, 1.0f, 0.0f);
	ChangeColorSoftly(highlight_attack_arrow_range, attack_arrow_range_renderer, highlight_attack_arrow_range_duration, false, false, false, true, 1.0f, 0.0f);
	ChangeColorSoftly(highlight_attack_arrow_melee, attack_arrow_melee_renderer, highlight_attack_arrow_melee_duration, false, false, false, true, 1.0f, 0.0f);
	ChangeColorSoftly(highlight_knockback_arrow, knockback_arrow_renderer, highlight_knockback_arrow_duration, false, false, false, true, 1.0f, 0.0f);
	ChangeColorSoftly(highlight_action_and_damage, action_and_damage_renderer, highlight_action_and_damage_duration, false, false, false, true, 1.0f, 0.0f);
	ChangeColorSoftly(highlight_tile_select, tile_select_renderer, highlight_tile_select_duration, false, false, false, true, 1.0f, 0.0f);

	UpdateFlickering(flicker_hp, healthComp, decreased_hp_amount_on_flickering);
}

void Highlightable_Monster::OnAnotherPhase()
{
	highlight_sprite = true;
	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_knockback_arrow = false;
	highlight_action_and_damage = false;
	highlight_tile_select = false;
	flicker_hp = false;
}

void Highlightable_Monster::OnAnotherPhase_Hover()
{
	highlight_sprite = true;
	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_knockback_arrow = false;
	highlight_action_and_damage = false;
	highlight_tile_select = false;
	flicker_hp = false;
}

void Highlightable_Monster::OnTacticMode_Idle()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	highlight_tile_select = false;
	flicker_hp = false;

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;

	if (mobComp->GetActType() == ActType::Move)
	{
		highlight_move_arrow = true;
	}
	else if (mobComp->GetActType() == ActType::Attack)
	{
		if (mobComp->GetEnemyType() == EnemyType::Far)
		{
			highlight_attack_arrow_range = true;
		}
		else
		{
			highlight_attack_arrow_melee = true;
		}
	}
}

void Highlightable_Monster::OnTacticMode_Idle_Hover()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	highlight_tile_select = true;
	flicker_hp = false;

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;

	if (mobComp->GetActType() == ActType::Move)
	{
		highlight_move_arrow = true;
	}
	else if (mobComp->GetActType() == ActType::Attack)
	{
		highlight_action_and_damage = true;
		if (mobComp->GetEnemyType() == EnemyType::Far)
		{
			highlight_attack_arrow_range = true;
		}
		else
		{
			highlight_attack_arrow_melee = true;
		}
	}
}

void Highlightable_Monster::OnTacticMode_Aim_Gun()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	highlight_tile_select = false;
	flicker_hp = false;

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;

	if (mobComp->GetActType() == ActType::Move)
	{
		highlight_move_arrow = true;
	}
	else if (mobComp->GetActType() == ActType::Attack)
	{
		if (mobComp->GetEnemyType() == EnemyType::Far)
		{
			highlight_attack_arrow_range = true;
		}
		else
		{
			highlight_attack_arrow_melee = true;
		}
	}
}

void Highlightable_Monster::OnTacticMode_Aim_Gun_Hover()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	highlight_tile_select = true;

	flicker_hp = true;
	decreased_hp_amount_on_flickering = 1;

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;

	if (mobComp->GetActType() == ActType::Move)
	{
		highlight_move_arrow = true;
	}
	else if (mobComp->GetActType() == ActType::Attack)
	{
		highlight_action_and_damage = true;
		if (mobComp->GetEnemyType() == EnemyType::Far)
		{
			highlight_attack_arrow_range = true;
		}
		else
		{
			highlight_attack_arrow_melee = true;
		}
		ThisIsMyDamnVictim();
	}
}

void Highlightable_Monster::OnTacticMode_Aim_Crossbow()
{
	highlight_sprite = false;
	highlight_knockback_arrow = false;
	highlight_tile_select = false;
	flicker_hp = false;

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;

	if (mobComp->GetActType() == ActType::Move)
	{
		highlight_move_arrow = true;
	}
	else if (mobComp->GetActType() == ActType::Attack)
	{
		if (mobComp->GetEnemyType() == EnemyType::Far)
		{
			highlight_attack_arrow_range = true;
		}
		else
		{
			highlight_attack_arrow_melee = true;
		}
	}
}

void Highlightable_Monster::OnTacticMode_Aim_Crossbow_Hover()
{
	highlight_sprite = true;
	highlight_knockback_arrow = true;

	int playerIdx = player->moveTargetTileIndex;
	int enemyIdx = GetOwner<GameObject>()->GetCurrentTileIndex();
	enemyIdx = playerIdx == enemyIdx ? player->GetCurrentTileIndex() : enemyIdx;

	int knockback_dir = playerIdx - enemyIdx > 0 ? -1 : 1;

	HighlightManager::SetArrowScaleAndAnchorByFaceDirection(GetOwner<GameObject>(), knockback_arrow_renderer, knockback_dir, 1.0f);

	highlight_tile_select = true;

	flicker_hp = false;
	int knockbackedIdx = enemyIdx + knockback_dir;
	if (knockbackedIdx < 0 || knockbackedIdx >= gameManager->InitTileSize)
	{
		flicker_hp = true;
		decreased_hp_amount_on_flickering = 1;
	}
	else if (GameObject* victim = gameManager->GetTile(knockbackedIdx)->object; victim != nullptr)
	{
		flicker_hp = true;
		decreased_hp_amount_on_flickering = 1;

		// 넉백 충돌의 희생양은 플레이어가 될 수 없음
		if (victim->GetComponent<MonsterComponent>()->IsBossMonster())
		{
			Highlightable_Boss* victims_highlightable = victim->GetComponent<Highlightable_Boss>();

			if (victims_highlightable != nullptr)
			{
				victims_highlightable->highlight_sprite = true;
				victims_highlightable->flicker_hp = true;
				victims_highlightable->decreased_hp_amount_on_flickering = 1;
			}
		}
		else
		{
			Highlightable_Monster* victims_highlightable = victim->GetComponent<Highlightable_Monster>();

			if (victims_highlightable != nullptr)
			{
				victims_highlightable->highlight_sprite = true;
				victims_highlightable->flicker_hp = true;
				victims_highlightable->decreased_hp_amount_on_flickering = 1;
			}
		}
	}

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;

	if (mobComp->GetActType() == ActType::Move)
	{
		highlight_move_arrow = true;
	}
	else if (mobComp->GetActType() == ActType::Attack)
	{
		highlight_action_and_damage = true;
		if (mobComp->GetEnemyType() == EnemyType::Far)
		{
			highlight_attack_arrow_range = true;
		}
		else
		{
			highlight_attack_arrow_melee = true;
		}
		ThisIsMyDamnVictim();
	}
}

void Highlightable_Monster::OnNormalMode_Idle()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	highlight_tile_select = false;
	flicker_hp = false;

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;
}

void Highlightable_Monster::OnNormalMode_Idle_Hover()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	highlight_tile_select = true;
	flicker_hp = false;

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;

	if (mobComp->GetActType() == ActType::Move)
	{
		highlight_move_arrow = true;
	}
	else if (mobComp->GetActType() == ActType::Attack)
	{
		highlight_action_and_damage = true;
		if (mobComp->GetEnemyType() == EnemyType::Far)
		{
			highlight_attack_arrow_range = true;
		}
		else
		{
			highlight_attack_arrow_melee = true;
		}
	}
}

void Highlightable_Monster::OnNormalMode_Aim_Gun()
{
	highlight_sprite = false;
	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_knockback_arrow = false;
	highlight_action_and_damage = false;
	highlight_tile_select = false;
	flicker_hp = false;
}

void Highlightable_Monster::OnNormalMode_Aim_Gun_Hover()
{
	highlight_sprite = true;
	highlight_knockback_arrow = false;
	highlight_tile_select = true;

	flicker_hp = true;
	decreased_hp_amount_on_flickering = 1;

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;

	if (mobComp->GetActType() == ActType::Move)
	{
		highlight_move_arrow = true;
	}
	else if (mobComp->GetActType() == ActType::Attack)
	{
		highlight_action_and_damage = true;
		if (mobComp->GetEnemyType() == EnemyType::Far)
		{
			highlight_attack_arrow_range = true;
		}
		else
		{
			highlight_attack_arrow_melee = true;
		}
		ThisIsMyDamnVictim();
	}
}

void Highlightable_Monster::OnNormalMode_Aim_Crossbow()
{
	highlight_sprite = false;
	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_knockback_arrow = false;
	highlight_action_and_damage = false;
	highlight_tile_select = false;
	flicker_hp = false;
}

void Highlightable_Monster::OnNormalMode_Aim_Crossbow_Hover()
{
	highlight_sprite = true;
	highlight_knockback_arrow = true;

	int playerIdx = player->moveTargetTileIndex;
	int enemyIdx = GetOwner<GameObject>()->GetCurrentTileIndex();
	enemyIdx = playerIdx == enemyIdx ? player->GetCurrentTileIndex() : enemyIdx;

	int knockback_dir = playerIdx - enemyIdx > 0 ? -1 : 1;

	HighlightManager::SetArrowScaleAndAnchorByFaceDirection(GetOwner<GameObject>(), knockback_arrow_renderer, knockback_dir, 1.0f);

	highlight_tile_select = true;

	flicker_hp = false;
	int knockbackedIdx = enemyIdx + knockback_dir;
	if (knockbackedIdx < 0 || knockbackedIdx >= gameManager->InitTileSize)
	{
		flicker_hp = true;
		decreased_hp_amount_on_flickering = 1;
	}
	else if (GameObject* victim = gameManager->GetTile(knockbackedIdx)->object; victim != nullptr)
	{
		flicker_hp = true;
		decreased_hp_amount_on_flickering = 1;

		// 넉백 충돌의 희생양은 플레이어가 될 수 없음
		if (victim->GetComponent<MonsterComponent>()->IsBossMonster())
		{
			Highlightable_Boss* victims_highlightable = victim->GetComponent<Highlightable_Boss>();

			if (victims_highlightable != nullptr)
			{
				victims_highlightable->highlight_sprite = true;
				victims_highlightable->flicker_hp = true;
				victims_highlightable->decreased_hp_amount_on_flickering = 1;
			}
		}
		else
		{
			Highlightable_Monster* victims_highlightable = victim->GetComponent<Highlightable_Monster>();

			if (victims_highlightable != nullptr)
			{
				victims_highlightable->highlight_sprite = true;
				victims_highlightable->flicker_hp = true;
				victims_highlightable->decreased_hp_amount_on_flickering = 1;
			}
		}
	}

	highlight_move_arrow = false;
	highlight_attack_arrow_range = false;
	highlight_attack_arrow_melee = false;
	highlight_action_and_damage = false;

	if (mobComp->GetActType() == ActType::Move)
	{
		highlight_move_arrow = true;
	}
	else if (mobComp->GetActType() == ActType::Attack)
	{
		highlight_action_and_damage = true;
		if (mobComp->GetEnemyType() == EnemyType::Far)
		{
			highlight_attack_arrow_range = true;
		}
		else
		{
			highlight_attack_arrow_melee = true;
		}
		ThisIsMyDamnVictim();
	}
}

void Highlightable_Monster::ThisIsMyDamnVictim()
{
	
}


void Highlightable_Chandelier::Awake()
{
}

void Highlightable_Chandelier::Start()
{
	player = GetOwner()->GetOwner()->GetObjectByName<Player>("Player");
	gameManager = GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager");
	type = TargetType::Chandelier;

	sprite_renderer = GetOwner()->GetComponent<SpriteRenderer>();


	Object* obj;

	obj = GetOwner()->AddObject<Object>();
	obj->SetDrawingPriority(OrderLayer::indicator_default);
	obj->transform->SetParent(GetOwner()->transform);
	obj->transform->SetPositionY(-50.0f);
	attack_arrow_renderer = obj->AddComponent<SpriteRenderer>();
	attack_arrow_renderer->SetImage("arrow/down");
	attack_arrow_renderer->SetSizeAuto();
	attack_arrow_renderer->color.A = 0.0f;
	attack_arrow_renderer->enable = false;

	//GetOwner()->SetChildsDrawingPriority(OrderLayer::Chandelier);

	hugeEpicGorgeousHonorableGreatDamage = GetOwner<Trap>()->attackDamage;
}

void Highlightable_Chandelier::Update()
{
}

void Highlightable_Chandelier::LateUpdate()
{
	ChangeColorSoftly(highlight_sprite, sprite_renderer, highlight_sprite_duration, true, true, true, false, 1.0f, highlight_sprite_min_color);
	ChangeColorSoftly(highlight_attack_arrow, attack_arrow_renderer, highlight_attack_arrow_duration, false, false, false, true, 1.0f, 0.0f);
}

void Highlightable_Chandelier::OnAnotherPhase()
{
	highlight_sprite = true;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::OnAnotherPhase_Hover()
{
	highlight_sprite = true;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::OnTacticMode_Idle()
{
	highlight_sprite = true;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::OnTacticMode_Idle_Hover()
{
	highlight_sprite = true;
	highlight_attack_arrow = true;
	//HighlightBelowObject();
}

void Highlightable_Chandelier::OnTacticMode_Aim_Gun()
{
	highlight_sprite = false;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::OnTacticMode_Aim_Gun_Hover()
{
	highlight_sprite = true;
	highlight_attack_arrow = true;
	HighlightBelowObject();
}

void Highlightable_Chandelier::OnTacticMode_Aim_Crossbow()
{
	highlight_sprite = false;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::OnTacticMode_Aim_Crossbow_Hover()
{
	highlight_sprite = false;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::OnNormalMode_Idle()
{
	highlight_sprite = true;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::OnNormalMode_Idle_Hover()
{
	highlight_sprite = true;
	highlight_attack_arrow = true;
	//HighlightBelowObject();
}

void Highlightable_Chandelier::OnNormalMode_Aim_Gun()
{
	highlight_sprite = false;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::OnNormalMode_Aim_Gun_Hover()
{
	highlight_sprite = true;
	highlight_attack_arrow = true;
	HighlightBelowObject();
}

void Highlightable_Chandelier::OnNormalMode_Aim_Crossbow()
{
	highlight_sprite = false;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::OnNormalMode_Aim_Crossbow_Hover()
{
	highlight_sprite = false;
	highlight_attack_arrow = false;
}

void Highlightable_Chandelier::HighlightBelowObject()
{
	std::vector<TileObject*> tiles = gameManager->tileObjects;

	int player_curIdx = player->GetCurrentTileIndex();
	int player_moveIdx = player->moveTargetTileIndex;
	if (player_curIdx != player_moveIdx)
	{
		std::swap(tiles[player_curIdx], tiles[player_moveIdx]);
	}

	int idx = GetOwner<GameObject>()->GetCurrentTileIndex();
	GameObject* victim = tiles[idx]->object;

	if (victim != nullptr)
	{
		if (victim->type == EType::Player)
		{
			Highlightable_Player* hc = victim->GetComponent<Highlightable_Player>();
			hc->highlight_sprite = true;
			hc->flicker_hp = true;
			hc->decreased_hp_amount_on_flickering = hugeEpicGorgeousHonorableGreatDamage;
		}
		else if (victim->type == EType::Enemy && !victim->GetComponent<MonsterComponent>()->IsBossMonster())
		{
			Highlightable_Monster* hc = victim->GetComponent<Highlightable_Monster>();
			hc->highlight_sprite = true;
			hc->flicker_hp = true;
			hc->decreased_hp_amount_on_flickering = hugeEpicGorgeousHonorableGreatDamage;
		}
	}
}



void Highlightable_Gunpowder::Awake()
{
}

void Highlightable_Gunpowder::Start()
{
	player = GetOwner()->GetOwner()->GetObjectByName<Player>("Player");
	gameManager = GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager");
	type = TargetType::Gunpowder;

	sprite_renderer = GetOwner()->GetComponent<SpriteRenderer>();


	Object* obj;

	GetOwner()->SetChildsDrawingPriority(OrderLayer::GunPowder);

	obj = GetOwner()->AddObject<Object>();
	obj->transform->SetParent(GetOwner()->transform);
	obj->transform->SetPositionY(-sprite_renderer->GetHeight() * 0.45f);
	attack_area_renderer = obj->AddComponent<SpriteRenderer>();
	attack_area_renderer->SetImage("Gunpowder/BombArea");
	attack_area_renderer->SetSizeAuto();
	attack_area_renderer->color.A = 0.0f;
	attack_area_renderer->enable = false;

	obj->SetDrawingPriority(OrderLayer::GunPowder - 1);

	samgukji = GetOwner<Trap>()->attackDamage;
}

void Highlightable_Gunpowder::Update()
{
}

void Highlightable_Gunpowder::LateUpdate()
{
	ChangeColorSoftly(highlight_sprite, sprite_renderer, highlight_sprite_duration, true, true, true, false, 1.0f, highlight_sprite_min_color);
	ChangeColorSoftly(highlight_attack_area, attack_area_renderer, highlight_attack_area_duration, false, false, false, true, 1.0f, 0.0f);
}

void Highlightable_Gunpowder::OnAnotherPhase()
{
	highlight_sprite = true;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::OnAnotherPhase_Hover()
{
	highlight_sprite = true;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::OnTacticMode_Idle()
{
	highlight_sprite = true;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::OnTacticMode_Idle_Hover()
{
	highlight_sprite = true;
	highlight_attack_area = true;
	//HighlightAround();
}

void Highlightable_Gunpowder::OnTacticMode_Aim_Gun()
{
	highlight_sprite = false;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::OnTacticMode_Aim_Gun_Hover()
{
	highlight_sprite = true;
	highlight_attack_area = true;
	HighlightAround();
}

void Highlightable_Gunpowder::OnTacticMode_Aim_Crossbow()
{
	highlight_sprite = false;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::OnTacticMode_Aim_Crossbow_Hover()
{
	highlight_sprite = false;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::OnNormalMode_Idle()
{
	highlight_sprite = true;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::OnNormalMode_Idle_Hover()
{
	highlight_sprite = true;
	highlight_attack_area = true;
	//HighlightAround();
}

void Highlightable_Gunpowder::OnNormalMode_Aim_Gun()
{
	highlight_sprite = false;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::OnNormalMode_Aim_Gun_Hover()
{
	highlight_sprite = true;
	highlight_attack_area = true;
	HighlightAround();
}

void Highlightable_Gunpowder::OnNormalMode_Aim_Crossbow()
{
	highlight_sprite = false;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::OnNormalMode_Aim_Crossbow_Hover()
{
	highlight_sprite = false;
	highlight_attack_area = false;
}

void Highlightable_Gunpowder::HighlightAround()
{
	std::vector<TileObject*> tiles = gameManager->tileObjects;

	int player_curIdx = player->GetCurrentTileIndex();
	int player_moveIdx = player->moveTargetTileIndex;
	if (player_curIdx != player_moveIdx)
	{
		std::swap(tiles[player_curIdx], tiles[player_moveIdx]);
	}

	int idx = GetOwner<GameObject>()->GetCurrentTileIndex();

	GameObject* victim = tiles[idx]->object;
	if (victim != nullptr)
	{
		HighlightVictim(victim, -1);
	}
	if (idx + 1 < gameManager->InitTileSize)
	{
		victim = tiles[idx + 1]->object;
		if (victim != nullptr)
		{
			HighlightVictim(victim, 1);
		}
	}
}

void Highlightable_Gunpowder::HighlightVictim(GameObject* victim, int push_dir)
{
	if (victim != nullptr && victim->type == EType::Player)
	{
		Highlightable_Player* hc = victim->GetComponent<Highlightable_Player>();
		hc->highlight_sprite = true;
		hc->highlight_knockback_arrow = true;
		HighlightManager::SetArrowScaleAndAnchorByFaceDirection(victim, hc->knockback_arrow_renderer, push_dir, 1.0f);
		hc->flicker_hp = true;
		hc->decreased_hp_amount_on_flickering = samgukji;
	}
	else if (victim != nullptr && victim->type == EType::Enemy && !victim->GetComponent<MonsterComponent>()->IsBossMonster())
	{
		Highlightable_Monster* hc = victim->GetComponent<Highlightable_Monster>();
		hc->highlight_sprite = true;
		hc->highlight_knockback_arrow = true;
		HighlightManager::SetArrowScaleAndAnchorByFaceDirection(victim, hc->knockback_arrow_renderer, push_dir, 1.0f);
		hc->flicker_hp = true;
		hc->decreased_hp_amount_on_flickering = samgukji;
	}
}

void Highlightable_Boss::Awake()
{
}

void Highlightable_Boss::Start()
{
	player = GetOwner()->GetOwner()->GetObjectByName<Player>("Player");
	gameManager = GetOwner()->GetOwner()->GetObjectByName<GameManager>("GameManager");
	type = TargetType::Enemy;

	sprite_renderer = GetOwner()->GetComponent<SpriteRenderer>();
	mobComp = GetOwner()->GetComponent<MonsterComponent>();

	healthComp = GetOwner()->GetComponent<HealthComponent>();

	Object* obj;

	//GetOwner()->SetChildsDrawingPriority(OrderLayer::Enemy);
}

void Highlightable_Boss::Update()
{
}

void Highlightable_Boss::LateUpdate()
{
	ChangeColorSoftly(highlight_sprite, sprite_renderer, highlight_sprite_duration, true, true, true, false, 1.0f, highlight_sprite_min_color);

	UpdateFlickering(flicker_hp, healthComp, decreased_hp_amount_on_flickering);
}

void Highlightable_Boss::OnAnotherPhase()
{
	highlight_sprite = true;
	flicker_hp = false;
}

void Highlightable_Boss::OnAnotherPhase_Hover()
{
	highlight_sprite = true;
	flicker_hp = false;
}

void Highlightable_Boss::OnTacticMode_Idle()
{
	highlight_sprite = true;
	flicker_hp = false;
}

void Highlightable_Boss::OnTacticMode_Idle_Hover()
{
	highlight_sprite = true;
	flicker_hp = false;
}

void Highlightable_Boss::OnTacticMode_Aim_Gun()
{
	highlight_sprite = false;
	flicker_hp = false;
}

void Highlightable_Boss::OnTacticMode_Aim_Gun_Hover()
{
	highlight_sprite = true;

	flicker_hp = true;
	decreased_hp_amount_on_flickering = 1;

	if (GetOwner<GameObject>()->GetComponent<MonsterComponent>()->GetActType() == ActType::BossAttackReady)
	{
		ThisIsMyDamnVictim();
	}
}

void Highlightable_Boss::OnTacticMode_Aim_Crossbow()
{
	highlight_sprite = false;
	flicker_hp = false;
}

void Highlightable_Boss::OnTacticMode_Aim_Crossbow_Hover()
{
	highlight_sprite = false;
	flicker_hp = false;
}

void Highlightable_Boss::OnNormalMode_Idle()
{
	highlight_sprite = true;
	flicker_hp = false;

}

void Highlightable_Boss::OnNormalMode_Idle_Hover()
{
	highlight_sprite = true;
	flicker_hp = false;


}

void Highlightable_Boss::OnNormalMode_Aim_Gun()
{
	highlight_sprite = false;
	flicker_hp = false;
}

void Highlightable_Boss::OnNormalMode_Aim_Gun_Hover()
{
	highlight_sprite = true;

	flicker_hp = true;
	decreased_hp_amount_on_flickering = 1;


	if (GetOwner<GameObject>()->GetComponent<MonsterComponent>()->GetActType() == ActType::BossAttackReady)
	{
		ThisIsMyDamnVictim();
	}
}

void Highlightable_Boss::OnNormalMode_Aim_Crossbow()
{
	highlight_sprite = false;
	flicker_hp = false;
}

void Highlightable_Boss::OnNormalMode_Aim_Crossbow_Hover()
{
	highlight_sprite = false;
	flicker_hp = false;
}

void Highlightable_Boss::ThisIsMyDamnVictim()
{
	std::vector<TileObject*> tiles = gameManager->tileObjects;

	int player_curIdx = player->GetCurrentTileIndex();
	int player_moveIdx = player->moveTargetTileIndex;
	if (player_curIdx != player_moveIdx)
	{
		std::swap(tiles[player_curIdx], tiles[player_moveIdx]);
	}

	if (GameObject* victim = tiles[GetOwner<GameObject>()->GetCurrentTileIndex()]->object; victim != nullptr)
	{
		if (victim->type == EType::Player)
		{
			victim->GetComponent<Highlightable_Player>()->highlight_sprite = true;
		}
		else if (victim->type == EType::Enemy && !victim->GetComponent<MonsterComponent>()->IsBossMonster())
		{
			victim->GetComponent<Highlightable_Monster>()->highlight_sprite = true;
		}
	}
}
