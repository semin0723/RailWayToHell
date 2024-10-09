#pragma once

#include "Engine.h"

class Config : public ReflectObject
{
private:
	Config() = default;
	template <class T, typename std::enable_if_t<!std::is_abstract_v<T>, int>>
	friend ReflectObject* CreateObject();
	friend const Config& GetConfig();

	bool isLoaded;
public:
	int DEVELOPER_MODE;

	// UI -------------------------
	int UI_CursorSize;

	// Common -------------------------
	int Common_CollisionDamageBetweenCharacter;
	float Common_HitKnockbackDistance;
	float Common_HitKnockbackDuration;

	// Player -------------------------
	float Player_FirstMoveDuration;
	float Player_SecondMoveDuration;

	float Player_AttackEnterDelay;
	float Player_AttackHitStopDuration;
	float Player_AttackExitDelay;

	float Player_KnockbackEnterDelay;
	float Player_KnockbackHitStopDuration;
	float Player_KnockbackExitDelay;

	float Player_HitSpriteDuration;
	float Player_HitKnockbackDistance;

	int	Player_NormalAttackDamage;

	// Effect -------------------------
	float Effect_EnterShakeDurationOnPlayerAttack;
	float Effect_ExitShakeDurationOnPlayerAttack;
	float Effect_EnterShakeMagnitudeOnPlayerAttack;
	float Effect_ExitShakeMagnitudeOnPlayerAttack;

	float Effect_ShakeTrainInterval;
	float Effect_ShakeTrainDuration;
	float Effect_ShakeTrainMagnitude;

	// Monster -------------------------
	float Monster_KnockbackDuration;
	float Monster_HitSpriteDuration;

	float Monster_HitDelay;
	float Monster_MoveDelay;
	float Monster_NearAttackDelay;
	float Monster_FarAttackDelay;
	float Monster_RushDelay;

	// GameManager -------------------------
	float GameManager_MonsterSpawnTime;
	float GameManager_CommandWaitTime;
	float GameManager_AFKTextTime;
	int GameManager_StartStageNumber;


	// BackGround -------------------------
	float BackGround_ChageTime;

#define ConfigReflect(x)							\
	x(DEVELOPER_MODE)								\
	x(UI_CursorSize)								\
	x(Common_CollisionDamageBetweenCharacter)		\
	x(Common_HitKnockbackDistance)					\
	x(Common_HitKnockbackDuration)					\
	x(Player_FirstMoveDuration)						\
	x(Player_SecondMoveDuration)					\
	x(Player_AttackEnterDelay)						\
	x(Player_AttackHitStopDuration)					\
	x(Player_AttackExitDelay)						\
	x(Player_KnockbackEnterDelay)					\
	x(Player_KnockbackHitStopDuration)				\
	x(Player_KnockbackExitDelay)					\
	x(Player_HitSpriteDuration)						\
	x(Player_HitKnockbackDistance)					\
	x(Player_NormalAttackDamage)					\
	x(Effect_EnterShakeDurationOnPlayerAttack)		\
	x(Effect_ExitShakeDurationOnPlayerAttack)		\
	x(Effect_EnterShakeMagnitudeOnPlayerAttack)		\
	x(Effect_ExitShakeMagnitudeOnPlayerAttack)		\
	x(Effect_ShakeTrainInterval)					\
	x(Effect_ShakeTrainDuration)					\
	x(Effect_ShakeTrainMagnitude)					\
	x(Monster_KnockbackDuration)					\
	x(Monster_HitSpriteDuration)					\
	x(Monster_HitDelay)								\
	x(Monster_MoveDelay)							\
	x(Monster_NearAttackDelay)						\
	x(Monster_FarAttackDelay)						\
	x(Monster_RushDelay)							\
	x(GameManager_MonsterSpawnTime)					\
	x(GameManager_CommandWaitTime)					\
	x(GameManager_AFKTextTime)						\
	x(GameManager_StartStageNumber)					\
	x(BackGround_ChageTime)							\


	GENERATED_BODY(Config, ReflectObject)

};

const Config& GetConfig();
