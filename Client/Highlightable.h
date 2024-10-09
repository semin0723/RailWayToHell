#pragma once
#include "Engine.h"
#include "PlayerEnum.h"

class Player;
class GameObject;
class GameManager;
class MonsterComponent;

class HighlightManager : public Object
{
	GameManager* gameManager = nullptr;
	class TacticModeBtn* tacticBtn;
	Player* player = nullptr;
	GameObject* lastHoveredObject{ nullptr };
public:
	bool resetOnPhaseChange = false;

	GameObject* tutorial_target{ nullptr };

	void Start() override;
	void Update() override;
	void FixedUpdate() override;

	void UpdateHighlight();
	GameObject* UpdateHighlightStateAndFindHoveredTarget();

	// 적들의 행동이 결정된 후 화살표의 모양을 갱신한다.
	// 넉백은 화약통 때문에 실시간 갱신해야됨
	void UpdateIndicatorShape();

	// 화살표 방향 바꾸기 / 화살표랑 적 스프라이트 기본 방향이 같아야 함
	static void SetArrowScaleAndAnchorByFaceDirection(GameObject* subject, SpriteRenderer* arrow_renderer, int act_dir, float length);
	// 화살표 외 방향 바꾸기
	static void SetAnotherIndicatorScaleByFaceDirection(Object* subject, SpriteRenderer* indicator_renderer);
};



class Highlightable : public Script
{
public:
	enum class TargetType { Player, Enemy, Chandelier, Gunpowder };
protected:
	Player* player;
	GameManager* gameManager;
	TargetType type;
public:
	void ChangeColorSoftly(bool showing, SpriteRenderer* targetRenderer, float duration, bool change_r, bool change_g, bool change_b, bool change_a, float value_max = 1.0f, float value_min = 0.0f);
	void ChangeState(bool isTacticMode, bool isHover, PlayerPhase playerPhase, WeaponType weaponType);
	void UpdateFlickering(bool isFlickering, class HealthComponent* healthComp, int decresed_hp);

	// 플레이어 체력 깜빡거리는 건 하지 말자
	void IsPlayerAttacked();

	virtual void Awake() override {}
	virtual void Start() override;
	virtual void Update() override {}
	virtual void LateUpdate() override {}

	virtual void OnAnotherPhase() {}
	virtual void OnAnotherPhase_Hover() {}

	virtual void OnTacticMode_Idle() {}
	virtual void OnTacticMode_Idle_Hover() {}
	virtual void OnTacticMode_Aim_Gun() {}
	virtual void OnTacticMode_Aim_Gun_Hover() {}
	virtual void OnTacticMode_Aim_Crossbow() {}
	virtual void OnTacticMode_Aim_Crossbow_Hover() {}

	virtual void OnNormalMode_Idle() {}
	virtual void OnNormalMode_Idle_Hover() {}
	virtual void OnNormalMode_Aim_Gun() {}
	virtual void OnNormalMode_Aim_Gun_Hover() {}
	virtual void OnNormalMode_Aim_Crossbow() {}
	virtual void OnNormalMode_Aim_Crossbow_Hover() {}
};






// HighlightManager가 Update일 때 기본 하이라이팅 & 구조물 등 다른 오브젝트의 강조 상태에 영향을 주는 놈 처리 -> 상태가 완성된 뒤 LateUpdate에서 그리기
// 체력바 깜빡임은 HealthComponent로 이관 <- 맨 마지막에 함
class Highlightable_Player : public Highlightable
{
public:
	// 스프라이트
	bool highlight_sprite{ true };
	SpriteRenderer* sprite_renderer{ nullptr };
	float highlight_sprite_duration{ 0.3f };
	float highlight_sprite_min_color{ 0.5f };
	// 넉백 방향 화살표
	bool highlight_knockback_arrow{ false };
	SpriteRenderer* knockback_arrow_renderer{ nullptr };
	float highlight_knockback_arrow_duration{ 0.3f };
	// 체력바 깜빡임 간격
	bool flicker_hp{ false };
	class HealthComponent* healthComp{ nullptr };
	int decreased_hp_amount_on_flickering{ 0 };


	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	virtual void OnAnotherPhase();
	virtual void OnAnotherPhase_Hover();

	virtual void OnTacticMode_Idle();
	virtual void OnTacticMode_Idle_Hover();
	virtual void OnTacticMode_Aim_Gun();
	virtual void OnTacticMode_Aim_Gun_Hover();
	virtual void OnTacticMode_Aim_Crossbow();
	virtual void OnTacticMode_Aim_Crossbow_Hover();

	virtual void OnNormalMode_Idle();
	virtual void OnNormalMode_Idle_Hover();
	virtual void OnNormalMode_Aim_Gun();
	virtual void OnNormalMode_Aim_Gun_Hover();
	virtual void OnNormalMode_Aim_Crossbow();
	virtual void OnNormalMode_Aim_Crossbow_Hover();
};

class Highlightable_Monster : public Highlightable
{
	MonsterComponent* mobComp{ nullptr };
public:
	// 스프라이트
	bool highlight_sprite{ true };
	SpriteRenderer* sprite_renderer{ nullptr };
	float highlight_sprite_duration{ 0.3f };
	float highlight_sprite_min_color{ 0.5f };
	// 이동 방향 화살표
	bool highlight_move_arrow{ false };
	SpriteRenderer* move_arrow_renderer{ nullptr };
	float highlight_move_arrow_duration{ 0.3f };
	// 공격 방향 화살표(원거리)
	bool highlight_attack_arrow_range{ false };
	SpriteRenderer* attack_arrow_range_renderer{ nullptr };
	float highlight_attack_arrow_range_duration{ 0.3f };
	// 공격 방향 화살표(근접)
	bool highlight_attack_arrow_melee{ false };
	SpriteRenderer* attack_arrow_melee_renderer{ nullptr };
	float highlight_attack_arrow_melee_duration{ 0.3f };
	// 넉백 방향 화살표
	bool highlight_knockback_arrow{ false };
	SpriteRenderer* knockback_arrow_renderer{ nullptr };
	float highlight_knockback_arrow_duration{ 0.3f };
	// 행동과 데미지
	bool highlight_action_and_damage{ false };
	SpriteRenderer* action_and_damage_renderer{ nullptr };
	float highlight_action_and_damage_duration{ 0.3f };
	// 선택 타일 강조선
	bool highlight_tile_select{ false };
	SpriteRenderer* tile_select_renderer{ nullptr };
	float highlight_tile_select_duration{ 0.3f };
	// 체력바 깜빡임 간격
	bool flicker_hp{ false };
	class HealthComponent* healthComp{ nullptr };
	int decreased_hp_amount_on_flickering{ 0 };

	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	virtual void OnAnotherPhase();
	virtual void OnAnotherPhase_Hover();

	virtual void OnTacticMode_Idle();
	virtual void OnTacticMode_Idle_Hover();
	virtual void OnTacticMode_Aim_Gun();
	virtual void OnTacticMode_Aim_Gun_Hover();
	virtual void OnTacticMode_Aim_Crossbow();
	virtual void OnTacticMode_Aim_Crossbow_Hover();

	virtual void OnNormalMode_Idle();
	virtual void OnNormalMode_Idle_Hover();
	virtual void OnNormalMode_Aim_Gun();
	virtual void OnNormalMode_Aim_Gun_Hover();
	virtual void OnNormalMode_Aim_Crossbow();
	virtual void OnNormalMode_Aim_Crossbow_Hover();

	void ThisIsMyDamnVictim();
};

class Highlightable_Boss : public Highlightable
{
	MonsterComponent* mobComp{ nullptr };
public:
	// 스프라이트
	bool highlight_sprite{ true };
	SpriteRenderer* sprite_renderer{ nullptr };
	float highlight_sprite_duration{ 0.3f };
	float highlight_sprite_min_color{ 0.5f };
	// 체력바 깜빡임 간격
	bool flicker_hp{ false };
	class HealthComponent* healthComp{ nullptr };
	int decreased_hp_amount_on_flickering{ 0 };

	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	virtual void OnAnotherPhase();
	virtual void OnAnotherPhase_Hover();

	virtual void OnTacticMode_Idle();
	virtual void OnTacticMode_Idle_Hover();
	virtual void OnTacticMode_Aim_Gun();
	virtual void OnTacticMode_Aim_Gun_Hover();
	virtual void OnTacticMode_Aim_Crossbow();
	virtual void OnTacticMode_Aim_Crossbow_Hover();

	virtual void OnNormalMode_Idle();
	virtual void OnNormalMode_Idle_Hover();
	virtual void OnNormalMode_Aim_Gun();
	virtual void OnNormalMode_Aim_Gun_Hover();
	virtual void OnNormalMode_Aim_Crossbow();
	virtual void OnNormalMode_Aim_Crossbow_Hover();

	void ThisIsMyDamnVictim();
};

class Highlightable_Chandelier : public Highlightable
{
public:
	// 스프라이트
	bool highlight_sprite{ true };
	SpriteRenderer* sprite_renderer{ nullptr };
	float highlight_sprite_duration{ 0.3f };
	float highlight_sprite_min_color{ 0.5f };
	// 공격 방향 화살표
	bool highlight_attack_arrow{ false };
	SpriteRenderer* attack_arrow_renderer{ nullptr };
	float highlight_attack_arrow_duration{ 0.3f };

	int hugeEpicGorgeousHonorableGreatDamage = 2;

	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	virtual void OnAnotherPhase();
	virtual void OnAnotherPhase_Hover();

	virtual void OnTacticMode_Idle();
	virtual void OnTacticMode_Idle_Hover();
	virtual void OnTacticMode_Aim_Gun();
	virtual void OnTacticMode_Aim_Gun_Hover();
	virtual void OnTacticMode_Aim_Crossbow();
	virtual void OnTacticMode_Aim_Crossbow_Hover();

	virtual void OnNormalMode_Idle();
	virtual void OnNormalMode_Idle_Hover();
	virtual void OnNormalMode_Aim_Gun();
	virtual void OnNormalMode_Aim_Gun_Hover();
	virtual void OnNormalMode_Aim_Crossbow();
	virtual void OnNormalMode_Aim_Crossbow_Hover();

	void HighlightBelowObject();
};

class Highlightable_Gunpowder : public Highlightable
{
public:
	// 스프라이트
	bool highlight_sprite{ true };
	SpriteRenderer* sprite_renderer{ nullptr };
	float highlight_sprite_duration{ 0.3f };
	float highlight_sprite_min_color{ 0.5f };
	// 공격 범위 스프라이트
	bool highlight_attack_area{ false };
	SpriteRenderer* attack_area_renderer{ nullptr };
	float highlight_attack_area_duration{ 0.3f };

	int samgukji = 1;

	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	virtual void OnAnotherPhase();
	virtual void OnAnotherPhase_Hover();

	virtual void OnTacticMode_Idle();
	virtual void OnTacticMode_Idle_Hover();
	virtual void OnTacticMode_Aim_Gun();
	virtual void OnTacticMode_Aim_Gun_Hover();
	virtual void OnTacticMode_Aim_Crossbow();
	virtual void OnTacticMode_Aim_Crossbow_Hover();

	virtual void OnNormalMode_Idle();
	virtual void OnNormalMode_Idle_Hover();
	virtual void OnNormalMode_Aim_Gun();
	virtual void OnNormalMode_Aim_Gun_Hover();
	virtual void OnNormalMode_Aim_Crossbow();
	virtual void OnNormalMode_Aim_Crossbow_Hover();

	void HighlightAround();
	void HighlightVictim(GameObject* victim, int push_dir);
};

