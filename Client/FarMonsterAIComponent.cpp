#include "FarMonsterAIComponent.h"
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
#include "MonsterFarAttack.h"

/*
													����������������������������������������������������������
													|	     RootBehavior		|
													��	    (Select Node)		��
													����������������������������������������������������������
							 ��							��                                    ��
����������������������������������������������������������			����������������������������������������������������������											����������������������������������������������������������
|	     CheckPhase        	|			|	 IsEnemyRightInFront	|											|	   HasTargetInRange		|
��	   (Decorate Node) 		��			��	   (Decorate Node) 		��											��	    (Select Node)		��
����������������������������������������������������������			����������������������������������������������������������											����������������������������������������������������������
			  ��            						      ��             										  ��              ��                ��
����������������������������������������������������������			����������������������������������������������������������			����������������������������������������������������������   ����������������������������������������������������������   ����������������������������������������������������������
|	         Idle  	    	|			|	         Idle  	    	|			|	    TargetIsNone  		| 	|	   TargetIsMonster		|	|	   TargetIsPlayer		|
��	    (Action Node) 		��			��	    (Action Node) 		��			��	   (Decorate Node) 		�� 	��	   (Decorate Node)		��	��	   (Decorate Node)		��
����������������������������������������������������������			����������������������������������������������������������			����������������������������������������������������������	����������������������������������������������������������	����������������������������������������������������������
																						       ��                              ��                               ��
																				����������������������������������������������������������   ����������������������������������������������������������   ����������������������������������������������������������
																				|	         Move  	    	| 	|		    Move			|	|		  FarAttack		    |
																				��	    (Action Node) 		�� 	��	    (Action Node)		��	��	    (Action Node)		��
																				����������������������������������������������������������	����������������������������������������������������������	����������������������������������������������������������

*/

void FarMonsterAIComponent::Start()
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
	MonsterFarAttack* monsterFarAttack = new MonsterFarAttack;
	monsterFarAttack->SetParent(this);
	targetIsPlayer->AddNode(monsterFarAttack);

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

void FarMonsterAIComponent::Update()
{
	__super::Update();
}
