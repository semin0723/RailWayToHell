#include "NearMonsterAIComponent.h"
#include "HasTargetInRange.h"
#include "IsEnemyRightInFront.h"
#include "TargetIsMonster.h"
#include "TargetIsPlayer.h"
#include "TargetIsNone.h"
#include "CheckPhase.h"
#include "RootBehavior.h"
#include "MonsterIdle.h"
#include "MonsterMove.h"
#include "MonsterAttack.h"

/*
													忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
													|	     RootBehavior		|
													弛	    (Select Node)		弛
						 							戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
						     Ｉ							Ｉ                                    Ｋ
忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖			忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖											忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
|	     CheckPhase        	|			|	 IsEnemyRightInFront	|											|	   HasTargetInRange		|
弛	   (Decorate Node) 		弛			弛	   (Decorate Node) 		弛											弛	    (Select Node)		弛
戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎			戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎											戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
				⊿            							⊿             										  Ｉ              ⊿                Ｋ
忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖			忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖			忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖   忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖   忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
|	         Idle  	    	|			|	         Idle  	    	|			|	    TargetIsNone  		| 	|	   TargetIsMonster		|	|	   TargetIsPlayer		|
弛	    (Action Node) 		弛			弛	    (Action Node) 		弛			弛	   (Decorate Node) 		弛 	弛	   (Decorate Node)		弛	弛	   (Decorate Node)		弛
戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎			戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎			戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎	戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎	戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
																						   ⊿                              ⊿                               ⊿
																				忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖   忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖   忙式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
																				|	         Move  	    	| 	|		    Move			|	|		    Attack		    |
																				弛	    (Action Node) 		弛 	弛	    (Action Node)		弛	弛	    (Action Node)		弛
																				戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎	戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎	戌式式式式式式式式式式式式式式式式式式式式式式式式式式式戎

*/

void NearMonsterAIComponent::Start()
{
	CheckPhase* checkPhase = new CheckPhase;
	checkPhase->SetParent(this);
	
	IsEnemyRightInFront* enemyInFront = new IsEnemyRightInFront("IsEnemyRightInFront");
	enemyInFront->SetParent(this);

	MonsterIdle* actionMonsterIdle = new MonsterIdle;
	actionMonsterIdle->SetParent(this);
	MonsterIdle* actionMonsterIdle2 = new MonsterIdle;
	actionMonsterIdle2->SetParent(this);

	checkPhase->AddNode(actionMonsterIdle);
	enemyInFront->AddNode(actionMonsterIdle2);

	//=================================================================================

	HasTargetInRange* hasTargetInRange = new HasTargetInRange("HasTargetInRange");
	hasTargetInRange->SetParent(this);

	TargetIsNone* targetIsNone = new TargetIsNone("TargetIsNone");
	targetIsNone->SetParent(this);
	MonsterMove* monsterMove = new MonsterMove;
	monsterMove->SetParent(this);
	targetIsNone->AddNode(monsterMove);

	TargetIsMonster* targetIsmonster = new TargetIsMonster("TargetIsMonster");
	targetIsmonster->SetParent(this);
	// TODO: Add ActionNode
	MonsterMove* monsterMove2 = new MonsterMove;
	monsterMove2->SetParent(this);
	targetIsmonster->AddNode(monsterMove2);

	TargetIsPlayer* targetIsPlayer = new TargetIsPlayer("TargetIsPlayer");
	targetIsPlayer->SetParent(this);
	MonsterAttack* monsterAttack = new MonsterAttack;
	monsterAttack->SetParent(this);
	targetIsPlayer->AddNode(monsterAttack);

	hasTargetInRange->AttachChild(targetIsNone);
	hasTargetInRange->AttachChild(targetIsmonster);
	hasTargetInRange->AttachChild(targetIsPlayer);

	//=================================================================================

	RootBehavior* rootBehavior = new RootBehavior("RootBehavior");
	rootBehavior->SetParent(this);

	rootBehavior->AttachChild(checkPhase);
	rootBehavior->AttachChild(enemyInFront);
	rootBehavior->AttachChild(hasTargetInRange);

	SetRootNode(rootBehavior);
}

void NearMonsterAIComponent::Update()
{
	__super::Update();
}
