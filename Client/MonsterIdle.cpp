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

	// Idle ���¿����� �ƹ� �ൿ�� ���� ����.
	return CurStatus = BTStatus::Success;
}
