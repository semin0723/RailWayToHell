#include "BossAIComponent.h"
#include "SetPattern.h"
#include "AttackToPlayer.h"
#include "AttackTile.h"

void BossAIComponent::Start()
{
	SetPattern* setPattern = new SetPattern;
	setPattern->SetParent(this);

	AttackTile* attackTile = new AttackTile;
	attackTile->SetParent(this);

	AttackToPlayer* attackToPlayer = new AttackToPlayer;
	attackToPlayer->SetParent(this);

	setPattern->AttachChild(attackTile);
	setPattern->SetChildNodeProbability(0, 0.5f);

	setPattern->AttachChild(attackToPlayer);
	setPattern->SetChildNodeProbability(1, 0.5f);

	SetRootNode(setPattern);
}

void BossAIComponent::Update()
{
	__super::Update();
}
