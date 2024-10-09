#include "MonsterIdle.h"
#include "Engine.h"
#include "TestMonster.h"
#include "GameManager.h"
#include "MonsterComponent.h"

void MonsterIdle::Initialize()
{

}

BTStatus MonsterIdle::Behave()
{
	Initialize();

	// Idle 상태에서는 아무 행동도 하지 않음.
	return CurStatus = BTStatus::Success;
}
