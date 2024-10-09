#include <random>
#include "GameManager.h"
#include "SceneEffect.h"
#include "Cursor_Aim.h"
#include "GameObject.h"
#include "MonsterComponent.h"
#include "TestMonster.h"
#include "Player.h"
#include "ICommand.h"
#include "Shade.h"
#include "Trap.h"
#include "RushTest.h"
#include "FarMonster.h"
#include "HealthComponent.h"
#include "StageInfo.h"
#include "defines.h"
#include "PrintActionPriority.h"
#include "Highlightable.h"
#include "TurnLeft.h"
#include "SampleScene.h"
#include "TrashAnimation.h"
#include "SpawnWarning.h"
#include "TacticModeBtn.h"
#include "ScrollObject.h"
#include "PlayerText.h"
#include "ChatBalloonComponent.h"
#include "BossLeftHand.h"
#include "BossRightHand.h"
#include "Boss.h"
#include <format>
#include "GameOverUI.h"
#include "StageUI.h"
#include "TrainSpriteObject.h"
#include <filesystem>
#include "PauseMenu.h"
#include "Selectable.h"
#include "CreditScene.h"
#include "ClickMePlease.h"
#include "GameState.h"


class GameFlowBehaviorTree : public BT::BehaviourTreeV2
{
public:
	void Init()
	{
		Archive archive;
		Archive archive2;

		{
			YAML::Emitter emitter;
			emitter << YAML::BeginSeq;

			emitter << LoadArchive("GameFlow_IsStageClearNode:");
			emitter << LoadArchive("GameFlow_GameClearNode:"); 
			emitter << LoadArchive("GameFlow_StageStartNode:");

			emitter << YAML::EndSeq;

			archive2["BT::ForceSuccess"]["nextNode"]["BT::SequenceNode"]["childNodes"] = LoadArchive(emitter.c_str());
		}
		{
			YAML::Emitter emitter;
			emitter << YAML::BeginSeq;

			emitter << archive2; 
			emitter << LoadArchive("GameFlow_MonsterSpawnActionNode:");
			emitter << archive2;
			emitter << LoadArchive("GameFlow_DecideEnemyActionNode:");
			emitter << LoadArchive("GameFlow_PlayerActionNode:");
			emitter << LoadArchive("GameFlow_ProcessCommandNode:");

			emitter << YAML::EndSeq;

			archive["rootNode"]["BT::SequenceNode"]["childNodes"] = LoadArchive(emitter.c_str());
		}
		
		std::stringstream tempsstream;
		tempsstream << archive;
		//auto temp = tempsstream.str();
		DeSerialize(LoadArchive(tempsstream));
	}

#define GameFlowBehaviorTreeReflect(x)
	GENERATED_BODY(GameFlowBehaviorTree, BehaviourTreeV2)
};

class MonsterSpawner : public Object
{
public:
	virtual void Awake() override
	{
		for (int i = 0; i <= 29; i++)
		{
			spawnWaitEffect.AddFrame("Effect/SpawnWaitEffect" + std::to_string(i));
		}

		SpriteRenderer* sprite = AddComponent<SpriteRenderer>();
		sprite->SetImage("Effect/SpawnWaitEffect");
		sprite->anchorType = AnchorType::Center;
		//sprite->SetSizeAuto();
		sprite->SetSize(384.f, 216.f);

		sprite->color.G = 0.5f;
		sprite->color.B = 0.5f;

		animator = AddComponent<Animator>();
		animator->SetAnimation(spawnWaitEffect);

		SetDrawingPriority(OrderLayer::DefaultObject);

		warningSign = AddObject<SpawnWarning>();
		warningSign->SetDrawingPriority(OrderLayer::warning_sign);
	}
	void Start() override
	{
		if (spawnID == 100 || spawnID == 200)
		{
			animator->enable = false;
			GetComponent<SpriteRenderer>()->enable = false;
			warningSign->GetComponent<SpriteRenderer>()->enable = false;
		}
	}
	void Spawn()
	{
		GameObject* checkObj = gameManager->AddMonsterInTileByID(spawnID, index, weight);

		if (spawnID != 100 && spawnID != 200 && checkObj != nullptr) {
			GameEffect* spawnEffect = AddObject<GameEffect>();
			spawnEffect->CreateEffect(
				"Effect/SpawnEffect",
				Vector3(transform->GetPosition().x, transform->GetPosition().y, 0)
			);
			// 0.2초 생성 -> 1.05초 대기 -> 0.2초 제거
			spawnEffect->GetComponent<SpriteRenderer>()->SetSize(384.f, 216.f);
			spawnEffect->GetComponent<SpriteRenderer>()->color.A = 0.0f;

			int count = spawnEffect->GetComponent<Animator>()->GetCurrentAnimation()->GetFrameCount();
			float total_duration = count * spawnEffect->GetComponent<Animator>()->GetInterval();

			float showingTime = total_duration * 0.2f;
			float erasingTime = total_duration * 0.2f;
			float idleTime = total_duration - showingTime - erasingTime;
			spawnEffect->AddTimerFunction([=](bool lu)
				{
					spawnEffect->GetComponent<SpriteRenderer>()->color.A += Time::GetDeltaTime() / showingTime;
					if (!lu) return;
					spawnEffect->AddTimerFunction([=](bool lu)
						{
							spawnEffect->AddTimerFunction([=](bool lu)
								{
									spawnEffect->GetComponent<SpriteRenderer>()->color.A -= Time::GetDeltaTime() / erasingTime;
									if (!lu) return;
									spawnEffect->GetComponent<SpriteRenderer>()->color.A = 0.0f;
								}, true, erasingTime, 1);
						}, false, idleTime, 1);
				}, true, showingTime, 1);
		}



		warningSign->Kill();
		Kill();
	}

	void SetSpawnID(int id) {
		spawnID = id;

		switch (id) {
		case 1:
			warningSign->SetMonsterKey("Rush");
			break;
		case 2:
			warningSign->SetMonsterKey("Melee"); 
			break;
		case 3:
			warningSign->SetMonsterKey("Range");
			break;
		default:
			warningSign->SetMonsterKey("Range");
			break;
		}

		warningSign->transform->SetParent(transform);
	}
public:
	int index;
	int spawnID;
	int weight;
	GameManager* gameManager;

	Animator* animator;
	Animation spawnWaitEffect;
	SpawnWarning* warningSign;
};

void GameManager::Awake()
{
	GameManager* gm = GetOwner()->GetObjectByName<GameManager>("GameManager");


	AddObject<SceneEffect>("SceneEffect");
	AddObject<ClickMePlease>();

	// UI Create Area

	waveInfoUI = AddUIObject<NextWaveInfoPannel>();
	stageGoalUI = AddUIObject<StageGoalPannel>();

	stageGoalUI->SetTotalSpawnMonster(totalStageMonsterCount);
	stageGoalUI->SetCurrentKillMonster(currentKillMonsterCount);

	//AddObject<TurnLeft>();
	AddUIObject<StageUI>();
	AddUIObject<TacticModeBtn>("TacticModeBtn");
	cursor_aim = GetOwner()->GetObjectByName<Cursor_Aim>("Cursor");
	cursor_aim->SetGunCursor();

	//===============
	shade = AddObject<Shade>();
	shade->SetEnable(false);
	highlightManager = AddObject<HighlightManager>("HighlightManager");

	//================
	pauseMenu = AddUIObject<PauseMenu>();
	pauseMenu->gameManager = this;
	pauseMenu->SetEnable(false);

	for (size_t i = 0; i < InitTileSize; i++)
	{
		tileObjects.push_back(AddObject<TileObject>());
	}
	for (size_t i = 0; i < InitTileSize; i++)
	{
		tileObjects[i]->transform->SetPosition(GetTilePosition(i));
	}

	//GameEffect* trainSpark = AddObject<GameEffect>();
	//trainSpark->SetDrawingPriority(OrderLayer::train_spark);
	//trainSpark->GetComponent<SpriteRenderer>()->SetSize(250, 250);
	////trainSpark->GetComponent<SpriteRenderer>()->SetSizeAuto();
	//trainSpark->CreateEffect("VFX/TrainSpark", Vector3(-780.f, -490.f, 0));

	player = AddGameObjectInTile<Player>(playerInitIndex, AddObject< Player>("Player"));


	Restart();
	SetDrawingPriority(INT_MIN);



	backGroundMsusic = GetOwner()->GetObjectByName<BackGroundMusic>("BackGroundMusic")->backGroundMsusic;
	//AddComponent<AudioManager>()->SetBankSound("Sound", "SFX_PLAYER_DEATH");
	//GetComponent<AudioManager>()->Play();
}

void GameManager::OnDestroy()
{
	if (gameFlowBT)
	{
		delete gameFlowBT;
		gameFlowBT = nullptr;
	}
	for (auto& item : StageInfos2Original)
	{
		delete item;

	}
	ClearCommandList();
	ClearCommandList();

	
}

void GameManager::Start()
{


}

void GameManager::FixedUpdate()
{

}



void GameManager::Update()
{
	if (!isGameOver)
	{
		gameFlowBT->Behave(Time::GetDeltaTime());
		stageGoalUI->SetCurrentKillMonster(currentKillMonsterCount);
	}
	
	if (GetConfig().DEVELOPER_MODE == 1)
	{
		if (Input::IsDown(KeyCode::PageUp)) // 다음 스테이지
		{
			if (currentPhase == Phase::Player_Action)
			{
				for (auto& tileObject : tileObjects)
				{
					DestroyObjectInTile(tileObject->object);
					DestroyObjectInTile(tileObject->chandelier);
					DestroyObjectInTile(tileObject->gunPowder);
				}
				DestroyObjectInTile(stageLockObject);
				DestroyObjectInTile(bossMonster);
				stageLockObject = nullptr;
				bossMonster = nullptr;
				NextPhase();
			}
		}
		else if (Input::IsDown(KeyCode::PageDown)) // 이전 스테이지
		{
			if (currentPhase == Phase::Player_Action)
			{
				currentStage -= 2;

				for (auto& tileObject : tileObjects)
				{
					DestroyObjectInTile(tileObject->object);
					DestroyObjectInTile(tileObject->chandelier);
					DestroyObjectInTile(tileObject->gunPowder);
				}
				DestroyObjectInTile(stageLockObject);
				DestroyObjectInTile(bossMonster);
				stageLockObject = nullptr;
				bossMonster = nullptr;
				NextPhase();
			}
		}
		else if (Input::IsDown(KeyCode::End))
		{
			if (currentPhase == Phase::Player_Action) // 튜토리얼 스킵
			{
				SkipTutorial();
			}
		}
		if (Input::IsDown(KeyCode::Space))
		{
			Time::SetSpeedMult(5.0f);
		}
		if (Input::IsUp(KeyCode::Space))
		{
			Time::SetSpeedMult(1.0f);
		}
	}
	
	
	if (Input::IsDown(KeyCode::Esc) && pauseMenu->GetEnable() == false)
	{
		pauseMenu->OpenMenu();
	}
	else if (Input::IsDown(KeyCode::Esc) && pauseMenu->GetEnable() == true)
	{
		pauseMenu->CloseMenu();
	}

	

	
	
}

void GameManager::ClearStage()
{
	if (stageInfos.size())
	{
		GetOwner()->GetObjectByName<GameState>("GameState")->current_stage_number = currentStage;
		currentStageInfo = stageInfos[++currentStage];

		if (!currentStageInfo)
		{
			GameClear();
		}
		currentTurn = 0;

		for (auto& tileObject : tileObjects)
		{
			DestroyObjectInTile(tileObject->object);
			DestroyObjectInTile(tileObject->chandelier);
			DestroyObjectInTile(tileObject->gunPowder);
		}



		for (auto& item : monsterSpawners)
		{
			item.second->Kill();
		}
		monsterSpawners.clear();


		backGroundMsusic->SetParameter("Stage", currentStage);

		ScrollBackGround* backGround = GetOwner()->GetObjectByName<ScrollBackGround>("ScrollBackGround");
		backGround->SetBackGround(GetBackGround(currentStage));
	}
}

void GameManager::NextStage()
{
	if (stageInfos.size())
	{
		auto& structureInfo = StructureInfos[currentStage];

		for (size_t i = 0; i < structureInfo.Chandelier.size(); i++)
		{
			if (structureInfo.Chandelier[i])
			{
				AddGameObjectInTile<Chandelier>(i);
			}
		}

		for (size_t i = 0; i < structureInfo.GunPowder.size(); i++)
		{
			if (structureInfo.GunPowder[i])
			{
				AddGameObjectInTile<GunPowder>(i + 1);
			}
		}

		SpawnMonsterSpawner();

		prevPlayerIndex = GetPlayer()->GetCurrentTileIndex();

		GetPlayer()->MoveToTile({ .tileIndex = 3, .duration = 0.0f, .isSwap = true });

		totalStageMonsterCount = 0;
		currentKillMonsterCount = 0;
		for (int i = 1; i <= stageInfos[currentStage]->GetMaxTurn(); i++) {
			std::vector<int> spawnInfo, weightInfo;
			stageInfos[currentStage]->GetSpawnPos(spawnInfo, weightInfo, i);
			for (int spawn : spawnInfo) {
				if (spawn != -1) totalStageMonsterCount++;
			}
		}
		for (int i = 1; i <= stageInfos[currentStage]->GetMaxTurn(); i++) {
			if (stageInfos[currentStage]->GetTrackingSpawn(i) != -1) {
				totalStageMonsterCount++;
			}
		}
		stageGoalUI->SetTotalSpawnMonster(totalStageMonsterCount);
		stageGoalUI->SetCurrentKillMonster(currentKillMonsterCount);

	}

}
void GameManager::KillThemAll()
{
	for (auto tile : tileObjects)
	{
		if (tile->object == nullptr) continue;
		if (tile->object->type == EType::Enemy)
		{
			tile->object->GetComponent<HealthComponent>()->Hit(100);
		}
	}
}
void GameManager::NextTurn()
{
	for (int i = 0; i < tileObjects.size(); i++) {
		if (tileObjects[i]->object == nullptr) {
			printf("nullptr  ");
		}
		else if (tileObjects[i]->object != nullptr) {
			printf("Object  ");
		}
	}
	printf("\n");

	waveInfoUI->TurnEnd();
	stageGoalUI->SetCurrentTurn(stageInfos[currentStage]->GetMaxTurn() - currentTurn);
	++currentTurn;
	if (bossMonster != nullptr && bossMonster->IsEyeOpen() == true && bossMonster->GetAnimationState() == BossAnimationState::Idle) {
		bossMonster->IncreaseOpenWaitTurn();
		if (bossMonster->waitTurn == 2)
		{
			// 나 너무 힘들다
			bossMonster->isBossStageReady = false;
			isBossStageReady = false;
		}
	}
	if (currentStageInfo->GetMaxTurn() > 0 && currentStageInfo->GetMaxTurn() < currentTurn)
	{
		GameOver();
	}
	for (auto& monster : monsterObjects)
	{
		monster->GetComponent<MonsterComponent>()->SetActive(true);
		monster->GetComponent<MonsterComponent>()->SetActDirection();
		monster->SetAnimation("Idle");
	}

	if(bossMonster != nullptr) bossMonster->GetComponent<MonsterComponent>()->SetActive(true);

	waveInfoUI->TurnStart(stageInfos[currentStage], currentTurn + 1);
}

void GameManager::LoadStageFile(const char* filename)
{
	StageInfosOriginal.clear();
	StageInfos2Original.clear();
	stageInfos.clear();
	StructureInfos.clear();
	trainInfo.clear();


	DataAsset::LoadDataAsset<StageInfo>("Resource/StageInfo/StageData.csv", StageInfosOriginal);

	ReflectObject::LoadFiles<StageInfo2>("Resource/StageInfo/Stage.yaml", StageInfos2Original);

	std::vector<StructureInfo> tempStructureInfos;
	DataAsset::LoadDataAsset<StructureInfo>("Resource/StageInfo/ObjectData.csv", tempStructureInfos);


	std::vector<TrainInfo> tempTrainInfoInfos;
	DataAsset::LoadDataAsset<TrainInfo>("Resource/StageInfo/TrainData.csv", tempTrainInfoInfos);

	StageInfosOriginal.emplace_back();

	for (StageInfo& item : StageInfosOriginal)
	{
		stageInfos.emplace(item.GetStage(), &item);
	}
	for (StageInfo2*& item : StageInfos2Original)
	{
		stageInfos.emplace(item->GetStage(), item);
		item->BePrepare();
	}
	for (auto& item : tempStructureInfos)
	{
		StructureInfos.emplace(item.Stage, std::move(item));
	}
	for (auto& item : tempTrainInfoInfos)
	{
		trainInfo.emplace(item.Stage, std::move(item));
	}

}

void GameManager::Restart()
{
	player->SetObjectLayer(ObjectLayer::Object);
	player->SetDrawingPriority(OrderLayer::Player);
	
	ClearCommandList();
	ClearCommandList();



	GameState* gameState = GetOwner()->GetObjectByName<GameState>("GameState");
	if (gameState->isRestart == true)
	{
		gameState->isRestart = false;
		currentStage = gameState->current_stage_number;
	}
	else
	{
		currentStage = GetConfig().GameManager_StartStageNumber;
	}



	currentPhase = Phase::StageClear;
	currentTurn = 1;
	for (auto& tileObject : tileObjects)
	{
		DestroyObjectInTile(tileObject->object);
		DestroyObjectInTile(tileObject->chandelier);
		DestroyObjectInTile(tileObject->gunPowder);
	}
	DestroyObjectInTile(stageLockObject);
	DestroyObjectInTile(bossMonster);
	stageLockObject = nullptr;
	bossMonster = nullptr;

	if (gameFlowBT)
	{
		delete gameFlowBT;
	}
	GameFlowBehaviorTree* _gameFlowBT = new GameFlowBehaviorTree();
	_gameFlowBT->Init();
	_gameFlowBT->SetOwner(this);
	gameFlowBT = _gameFlowBT;

	isGameOver = false;
	if (newtrain)
	{
		newtrain->Kill();
		newtrain = nullptr;
	}
	if (maintrain)
	{
		maintrain->Kill();
		maintrain = nullptr;
	}
	if (oldtrain)
	{
		oldtrain->Kill();
		oldtrain = nullptr;
	}
	if (tempLeftTrain)
	{
		tempLeftTrain->Kill();
		tempLeftTrain = nullptr;
	}
	if (tempRightTrain)
	{
		tempRightTrain->Kill();
		tempRightTrain = nullptr;
	}
	player->ReStart();

	GetPlayer()->transform->SetPositionX(1200.0f);
}

void GameManager::PrepareRestart()
{
	GetOwner()->GetObjectByName<GameState>("GameState")->isRestart = true;
	SceneManager::LoadScene<SampleScene>();
}

void GameManager::SkipTutorial()
{
	if (GetCurrentStage() > 2) return;

	SetCurrentPhase(Phase::Type::Player_Action);
	SetCurrentStage(2);

	for (auto& tileObject : tileObjects)
	{
		DestroyObjectInTile(tileObject->object);
		DestroyObjectInTile(tileObject->chandelier);
		DestroyObjectInTile(tileObject->gunPowder);
	}

	NextPhase();
}

bool GameManager::CanMove(int index)
{
	if (!IsTileRangeIndex(index)) return false;
	return !tileObjects[index]->object;
}

Vector3 GameManager::GetTilePosition(int tileIndex)
{
	return Vector3{ (tileIndex - (tileObjects.size() - 1) * 0.5f) * 200.0f, -320.0f, 0.0f};
}

void GameManager::RegisterCommandList(int actSpeed, ICommand* command)
{
	commandList.emplace(actSpeed, command);
	if (GetCurrentPhase() != Phase::ProcessCommand)
	{
		currentCommand = commandList.begin();
	}
}

void GameManager::UnRegisterCommandList(ICommand* command)
{
	if (!command) return;
	if (commandList.empty()) return;
	auto findIter = std::find_if(commandList.begin(), commandList.end(), [command](const std::pair<const int, ICommand*>& element)
				   {
					   return (element.second == command);
				   });
	if (findIter == currentCommand && currentCommand != commandList.end())
	{
		currentCommand++;
	}
	if (findIter != commandList.end())
	{

		commandList.erase(findIter);
		delete command;
	}
	else
	{
	}
}

void GameManager::NextPhase()
{
	currentPhase = (Phase::Type)((currentPhase + 1) % Phase::MAX);
}

void GameManager::GameOver()
{
	isGameOver = true;

	GetOwner()->GetObjectByName<BackGroundMusic>("BackGroundMusic")->Stop();
	GetOwner()->GetObjectByName<BackGroundMusic>("BackGroundMusic")->SetDontDestroyOnSceneChange(false);
	player->SetObjectLayer(ObjectLayer::UI);
	player->SetDrawingPriority(OrderLayer::GameOver_ui);

	AddUIObject<GameOverUI>();
	int rand = Random::Range(11, 12);

	const PlayerText& text = PlayerTextManager().GetText(rand);
	GetPlayer()->GetComponent<ChatBalloonComponent>()->BubbleDialog(text.BubbleText, text.FontSize, text.BubbleDuration);
}

void GameManager::GameClear()
{
	isGameClear = true;
	//shade->SetEnable(true);


}


void GameManager::DestroyObjectInTile(GameObject* destroyObject)
{
	if (!destroyObject) return;
	if (stageLockObject == destroyObject)
	{
		if (tileObjects[destroyObject->currentTileindex]->object == destroyObject)
		{
			tileObjects[destroyObject->currentTileindex]->object = nullptr;
		}
		stageLockObject->Kill();
		stageLockObject = nullptr;
	}
	if (Chandelier* trap = dynamic_cast<Chandelier*>(destroyObject))
	{
		tileObjects[destroyObject->currentTileindex]->chandelier = nullptr;
		destroyObject->Kill();
	}
	else if (GunPowder* trap = dynamic_cast<GunPowder*>(destroyObject))
	{

		tileObjects[destroyObject->currentTileindex]->gunPowder = nullptr;
		destroyObject->Kill();
	}
	else if(MonsterComponent* monsterComp = destroyObject->GetComponent<MonsterComponent>(); monsterComp != nullptr)
	{
		if (monsterComp->GetEnemyType() == EnemyType::Boss_Left || monsterComp->GetEnemyType() == EnemyType::Boss_Right)
		{
			int curIdx = monsterComp->GetOwner<GameObject>()->GetCurrentTileIndex();
			if (tileObjects[curIdx]->object == destroyObject)
			{
					tileObjects[destroyObject->currentTileindex]->object = nullptr;
			}
		}
		else
		{
			if (destroyObject == tileObjects[destroyObject->currentTileindex]->object)
			{
				tileObjects[destroyObject->currentTileindex]->object = nullptr;
			}
		}

		monsterObjects.erase(std::remove(monsterObjects.begin(), monsterObjects.end(), destroyObject), monsterObjects.end());
		destroyObject->Kill();

	}
}

std::string GameManager::GetTrain(int index)
{
	auto iter = trainInfo.find(index);
	if (iter == trainInfo.end())
	{
		return "";
	}
	return iter->second.TrainName;
}

std::string GameManager::GetBackGround(int index)
{
	auto iter = trainInfo.find(index);
	if (iter == trainInfo.end())
	{
		return (--trainInfo.end())->second.BackGround;
	}
	return iter->second.BackGround;
}

MonsterSpawner* GameManager::GetSpawner(int index)
{
	for (auto& item : monsterSpawners)
	{
		if (item.second->index == index)
		{
			return item.second;
		}
	}
	return nullptr;
	
}

void GameManager::SpawnMonsterInTile(int targetIndex, int monsterId, int actWeight)
{
	if (targetIndex < 0 || targetIndex > 6) return;
	MonsterSpawner* spawner = AddObject<MonsterSpawner>();

	spawner->transform->SetPosition(GetTilePosition(targetIndex) + Vector3(5.0f, 60.0f, 0.0f));

	spawner->index = targetIndex;
	spawner->spawnID = monsterId;
	spawner->weight = actWeight;

	//spawner->transform->SetPosition(DEFINE::GetTilePositionX(targetIndex), -320.f, 0);

	spawner->gameManager = this;

	spawner->Spawn();
}

void GameManager::ClearCommandList()
{
	for (auto& item : prevcommandList)
	{
		delete item.second;
	}
	prevcommandList.clear();
	prevcommandList = std::move(commandList);
	currentCommand = commandList.end();
}

bool GameManager::ObjectMove(int prevIndex, int targetIndex, bool isSwap)
{
	if (!IsTileRangeIndex(prevIndex)) return false;
	if (!IsTileRangeIndex(targetIndex)) return false;
	if (!tileObjects[prevIndex]->object) return false;
	GameObject* tempObject = nullptr;
	if (isSwap)
	{	
		tempObject = tileObjects[targetIndex]->object;
	}
	else if (tileObjects[targetIndex]->object)
	{
		return false;
	}

	tileObjects[targetIndex]->object = tileObjects[prevIndex]->object;
	tileObjects[prevIndex]->object = tempObject;


	if (tempObject)
	{
		tileObjects[prevIndex]->object->currentTileindex = prevIndex;
		tempObject->MoveToTile({.tileIndex = prevIndex });
	}
	tileObjects[targetIndex]->object->currentTileindex = targetIndex;

	return true;
}

int GameManager::GetObjectIndex(GameObject* gamObject)
{
	if (gamObject)
	{
		for (size_t i = 0; i < tileObjects.size(); i++)
		{
			if (tileObjects[i]->object == gamObject)
			{
				return i;
			}
		}
	}
	return 0;
}


int GameManager::GetTrapIndex(Trap* trap)
{
	if (trap)
	{
		for (size_t i = 0; i < tileObjects.size(); i++)
		{
			if (tileObjects[i]->chandelier == trap)
			{
				return i;
			}
			if (tileObjects[i]->gunPowder == trap)
			{
				return i;
			}
		}
	}
	return 0;
}

int GameManager::GetLeftTurn()
{ 
	if (currentStageInfo)
	{
		return currentStageInfo->GetMaxTurn() - currentTurn + 1;
	}
	return 0;
}

void GameManager::DamageInTile(int index, int damage)
{
	if (GetTile(index) == nullptr) return;
	if (GetTile(index)->object == nullptr) return;

	GetTile(index)->object->GetComponent<HealthComponent>()->Hit(damage);
}

void GameManager::Knockback(bool triggeredFromPlayerAttack, GameObject* attackTarget, int attackDir)
{
	float monsterKnockbackTime = GetConfig().Monster_KnockbackDuration;
	int collisionDamage = 1;
	int targetIndex = attackTarget->GetCurrentTileIndex();
	int afterIndex = targetIndex + attackDir;

	if (CanMove(afterIndex)) // 밀려날 타일에 적이 있는지 체크
	{
		// 아무도 없다면 그냥 밀려남
		attackTarget->MoveToTile({ .tileIndex = afterIndex, .easing = Math::EaseOutCubic, .duration = monsterKnockbackTime});
	}
	else // 벽 또는 다른 적이 있는 경우
	{
		// 벽과 충돌할 경우, 본인만 피해
		if (afterIndex < 0)
		{
			attackTarget->KnockbackBounce(-1, 0, monsterKnockbackTime * 0.4f, monsterKnockbackTime * 0.6f);
			attackTarget->AddTimerFunction([attackTarget, collisionDamage](bool lu)
				{
					attackTarget->GetComponent<HealthComponent>()->Hit(collisionDamage);
				}, false, monsterKnockbackTime * 0.4f, 1);
		}
		else if (afterIndex >= GameManager::InitTileSize)
		{
			attackTarget->KnockbackBounce(GameManager::InitTileSize, GameManager::InitTileSize - 1, monsterKnockbackTime * 0.4f, monsterKnockbackTime * 0.6f);
			attackTarget->AddTimerFunction([attackTarget, collisionDamage](bool lu)
				{
					attackTarget->GetComponent<HealthComponent>()->Hit(collisionDamage);
				}, false, monsterKnockbackTime * 0.4f, 1);
		}
		else // 다른 적과 충돌할 경우, 둘 다 피해
		{
			attackTarget->KnockbackBounce(afterIndex, targetIndex, monsterKnockbackTime * 0.4f, monsterKnockbackTime * 0.6f);
			attackTarget->AddTimerFunction([attackTarget, collisionDamage](bool lu)
				{
					attackTarget->GetComponent<HealthComponent>()->Hit(collisionDamage);
				}, false, monsterKnockbackTime * 0.4f, 1);

			// 뒤에 있던 적은 부딪히는 순간 약간의 넉백 모션과 함께 피해를 입는다
			GameObject* victim = GetTile(afterIndex)->object;
			victim->AddTimerFunction([this, victim, attackDir, collisionDamage](bool lu)
				{
					victim->PlayHitKnockbackMotion(attackDir, GetConfig().Common_HitKnockbackDistance, GetConfig().Common_HitKnockbackDuration);
					victim->GetComponent<HealthComponent>()->Hit(collisionDamage);
				}, false, monsterKnockbackTime * 0.4f, 1);
		}

		// 임시 / 히트스톱은 넉백으로 인한 충돌이 일어날 때 발생
		if (triggeredFromPlayerAttack)
		{
			AddRealTimeTimerFunction([this](bool lu)
				{
					SceneEffect::PlayerAttackEnter(GetConfig().Effect_EnterShakeMagnitudeOnPlayerAttack, GetConfig().Effect_EnterShakeDurationOnPlayerAttack);
					AddRealTimeTimerFunction([this](bool lu)
						{
							SceneEffect::PlayerAttackExit(GetConfig().Effect_ExitShakeMagnitudeOnPlayerAttack, GetConfig().Effect_ExitShakeDurationOnPlayerAttack);
						}, false, GetConfig().Player_KnockbackHitStopDuration, 1);
				}, false, monsterKnockbackTime * 0.4f, 1);
		}
	}
}


bool GameManager::ProcessCommandList()
{
	if (currentCommand == commandList.end())
	{
		return false;
	}

	commandWaitTime += Time::GetDeltaTime();
	if (commandWaitTime <= GetConfig().GameManager_CommandWaitTime)
	{
		return true;
	}


	if (currentCommand->second->Do())
	{
		++currentCommand;
		commandWaitTime = 0.0f;

		return ProcessCommandList();
	}
	return true;
}

void GameManager::OnCharacterDie(GameObject* subject)
{
	if (subject->type == EType::Player)
	{
		// GameOver();
	}
	else if (subject->type == EType::Enemy)
	{
		// --enemy_count;
	}
}

struct CompareMonsterByActionOrder
{
	bool operator()(GameObject* a, GameObject* b) const
	{
		int aOrder = a->GetComponent<MonsterComponent>()->GetActionOrder();
		int bOrder = b->GetComponent<MonsterComponent>()->GetActionOrder();
		return aOrder < bOrder;
	}
};
void GameManager::UpdateIndicator()
{
	// 몬스터 위에 행동 순서 표시하기
	std::vector<GameObject*> tMonsters;
	for (auto tile : tileObjects)
	{
		if (tile->object == nullptr) continue;
		if (tile->object->type == EType::Player) continue;
		if (tile->object->type == EType::Else) continue;
		if (tile->object->type == EType::Enemy && tile->object->GetComponent<MonsterComponent>()->IsBossMonster()) continue;
		tMonsters.push_back(tile->object);
	}
	if (bossMonster != nullptr)
	{
		GameObject* hand = bossMonster->leftHand;
		if (hand != nullptr)
		{
			if (hand->GetComponent<MonsterComponent>()->GetActType() == ActType::BossAttackReady)
			{
				tMonsters.push_back(hand);
			}
		}
		hand = bossMonster->rightHand;
		if (hand != nullptr)
		{
			if (hand->GetComponent<MonsterComponent>()->GetActType() == ActType::BossAttackReady) tMonsters.push_back(hand);
		}
	}

	std::sort(tMonsters.begin(), tMonsters.end(), CompareMonsterByActionOrder());

	int priority = 1;
	bool isSecondBossHand = false;
	int boss_hand_priority = 1;
	for (auto monster : tMonsters)
	{
		if (monster->GetComponent<MonsterComponent>()->GetActType() == ActType::Idle) continue;

		if (monster->GetComponent<MonsterComponent>()->IsBossMonster() == true)
		{
			if (isSecondBossHand == false)
			{
				boss_hand_priority = priority;
				isSecondBossHand = true;
			}
			else if (isSecondBossHand == true)
			{
				monster->GetComponent<PrintActionPriority>()->PrintPriority(boss_hand_priority);
				continue;
			}
		}

		monster->GetComponent<PrintActionPriority>()->PrintPriority(priority);
		++priority;
	}

	highlightManager->UpdateIndicatorShape();
}

//spawner transform
void GameManager::SpawnMonsterSpawner() 
{
	std::vector<int> spawnPos;
	std::vector<int> spawnWeight;
	

	currentStageInfo->GetSpawnPos(spawnPos, spawnWeight, currentTurn + 1);
	if (!spawnPos.empty())
	{
		for (size_t i = 0; i < spawnPos.size(); i++)
		{
			if (spawnPos[i] != -1)
			{
				auto iter = monsterSpawners.emplace(currentTurn + 1, AddObject<MonsterSpawner>());
				iter->second->index = i;
				iter->second->SetSpawnID(spawnPos[i]);
				iter->second->weight =  spawnWeight[i];
				iter->second->gameManager = this;
				iter->second->transform->SetPosition(GetTilePosition(i) + Vector3(5.0f, 60.0f, 0.0f));
			}
		}
	}

	int spawnID = currentStageInfo->GetTrackingSpawn(currentTurn + 1);
	if (spawnID != -1)
	{
		auto iter = monsterSpawners.emplace(currentTurn + 1, AddObject<MonsterSpawner>());
		iter->second->index = GetPlayer()->GetCurrentTileIndex();
		iter->second->SetSpawnID(spawnID);
		iter->second->weight = 0;
		iter->second->gameManager = this;
		iter->second->transform->SetPosition(GetTilePosition(GetPlayer()->GetCurrentTileIndex()) + Vector3(5.0f, 60.0f, 0.0f));
	}


}

class TrainHeart : public GameObject
{
	class DieCommand : public ICommand
	{
	public:
		virtual bool Do() override
		{
			elapsedTime += Time::GetDeltaTime();
			
			if (elapsedTime > LimitTime)
			{
				return true;
			}
			return false;
		}
		float elapsedTime;
		float LimitTime;
		TrainHeart* _this;
		GameManager* gameManager;
	};

	class HitFN : public IOnHit
	{
	public:
		virtual void operator()(int damage) override
		{
			{
				GameEffect* hitEffect = _this->AddObject<GameEffect>();
				int randomNum = Random::Range(1, 3);
				hitEffect->CreateEffect("VFX/HitBlood" + std::to_string(randomNum), _this->transform->GetPosition() + Vector3(0, 100.f, 0));
				hitEffect->GetComponent<SpriteRenderer>()->SetSize(100.f, 100.f);
				hitEffect->SetDrawingPriority(OrderLayer::monster_hitEffect);
			}
			{
				GameEffect* hitEffect = _this->AddObject<GameEffect>();
				hitEffect->CreateEffect("VFX/DeadHit", _this->transform->GetPosition() + Vector3(0, 100.f, 0));
				//hitEffect->GetComponent<SpriteRenderer>()->SetSize(100.f, 100.f);
				hitEffect->GetComponent<SpriteRenderer>()->SetSizeAuto();
				hitEffect->SetDrawingPriority(OrderLayer::monster_hitEffect);
			}
			
			_this->heatBeat_channel->stop();
			AudioFramework::Play_BankSound("Sound", "SFX_Heart_Hurt");
			//AudioFramework::Play_BankSound("Sound", "BGM_End_Scene"); // <- ?
			//_this->gameManager->backGroundMsusic->Stop();

			Player* player = _this->gameManager->GetPlayer();

			SceneEffect* effect = _this->GetOwner()->GetObjectByName<SceneEffect>("SceneEffect");
			effect->ShakeCameraVertical(GetConfig().Effect_ShakeTrainMagnitude * 2.0f, 1.0f);
			effect->ShakeCameraHorizontal(GetConfig().Effect_ShakeTrainMagnitude * 2.0f, 1.0f);

			player->AddTimerFunction([effect](bool lastupdate) mutable
									 {
										 if (lastupdate)
										 {
											 effect->ShakeCameraVertical(GetConfig().Effect_ShakeTrainMagnitude * 1.0f, 0.5f);
											 effect->ShakeCameraHorizontal(GetConfig().Effect_ShakeTrainMagnitude * 1.0f, 0.5f);
										 }
									 }, false, 0.7f, 1);


			_this->gameManager->DestroyObjectInTile(_this);



			int tileIndex = _this->GetCurrentTileIndex();
			Vector3 _RelativePosition = _this->RelativePosition;
			int distanceIndex = tileIndex - 1 - player->GetCurrentTileIndex();

			player->MoveToPosition({ .tileIndex = tileIndex - 1, .RelativePosition = Vector3(-100.0f, 0.0f, 0.0f), .easing = &Math::Lerp,.duration = distanceIndex * 1.0f, .isSwap = true});



			player->AddTimerFunction([_RelativePosition, tileIndex, player](bool lastupdate) mutable
									 {
										 if (lastupdate)
										 {
											 player->MoveToPosition({ .tileIndex = tileIndex, .RelativePosition = _RelativePosition + Vector3(-200.0f, 0.0f, 0.0f),  .easing = &Math::Lerp,.duration = 1.4f, .isSwap = true});
										 }
									 }, false, distanceIndex * 1.0f, 1);

			player->AddTimerFunction([_RelativePosition, tileIndex, player](bool lastupdate) mutable
									 {
										 if (lastupdate)
										 {
											 player->MoveToPosition({ .tileIndex = tileIndex, .RelativePosition = _RelativePosition,  .easing = &Math::Lerp,.duration = 1.0f, .isSwap = true });
										 }
									 }, false, distanceIndex * 1.0f + 1.4f, 1);


			auto anim_Run = ResourceManager::GetAnimation("player/Run");
			anim_Run.get()->loop = true;
			player->animator->SetAnimation(*anim_Run);
			player->animator->enable = true;
			player->renderer->anchor = Vector2{ 0, 0 };
			player->renderer->SetSizeAuto();

			float elapsedTime = 0.0f;
			player->AddTimerFunction([player, elapsedTime](bool lastUpdate) mutable
									 {
										 if (!lastUpdate)
										 {
											 player->animator->enable = true;
											 elapsedTime += Time::GetDeltaTime() / 2.5f;
											 player->animator->SetInterval(Math::Lerp(0.1f, 0.02f, 1.0f - std::abs(1.0f - elapsedTime)));
										 }
										 else
										 {
											 player->animator->SetAnimation(*ResourceManager::GetAnimation("player/idle"));
											 player->animator->enable = true;
											 player->renderer->SetSizeAuto();
										 }
									 }, true, distanceIndex * 1.0f + 1.4f + 1.0f, 1);

			auto lever = _this->Lever;
			_this->Lever->AddTimerFunction([lever](bool lastUpdate) mutable
										   {
											   lever->GetComponent<SpriteRenderer>()->SetImage("Train/Lever2");
										   }, false, distanceIndex * 1.0f + 1.4f + 1.0f, 1);


			Object* object = _this->AddObject<Object>();
			_this->heartBackGround->transform->SetParent(object->transform);
			object->transform->SetPosition(_this->transform->GetPosition());
			SpriteRenderer* sprite = object->AddComponent<SpriteRenderer>();
			SpriteRenderer* sprite2 = _this->heartBackGround->GetComponent<SpriteRenderer>();
			sprite->anchorType = AnchorType::BottomCenter;

			sprite->SetImage("TrainHeart/Hit");
			sprite->SetSizeAuto();

			object->AddTimerFunction([object, sprite, sprite2](bool lastUpdate) mutable
												 {
													 sprite->SetImage("TrainHeart/Dead");
													 sprite->SetSizeAuto();
													 
													 object->AddRealTimeTimerFunction([=](bool lu)
																					  {
																						  sprite->color.A -= Time::GetDeltaTime();
																						  sprite2->color.A -= Time::GetDeltaTime();
																						  if (lu)
																						  {
																							  object->Kill();
																						  }
																					  }, true, 0.5f, 1);
												 }, false, 0.3f, 1);


			DieCommand* command = new DieCommand();
			command->gameManager = _this->gameManager;
			command->_this = _this;
			command->LimitTime = distanceIndex * 1.0f + 1.4f + 1.0f;
			_this->gameManager->RegisterCommandList(INT_MAX, command);


			for (size_t i = 0; i < _this->gameManager->InitTileSize; i++)
			{
				_this->gameManager->GetTile(i)->SetEnable(false);
			}
		}
		TrainHeart* _this;
	};
public:
	void Awake() override
	{

		RelativePosition = Vector3(-25.0f, 110.0f, 0.0f);

		type = EType::Else;

		sprite = AddComponent<SpriteRenderer>();
		sprite->anchorType = AnchorType::BottomCenter;

		animator = AddComponent<Animator>();
		animator->SetAnimation(*ResourceManager::GetAnimation("TrainHeart"));
		animator->enable = true;
		sprite->SetSizeAuto();

		heatBeat_channel = AudioFramework::Play_BankSound("Sound", "BGM_HeartAmbient", true);
		
		healthComponent = AddComponent<HealthComponent>();
		healthComponent->Init(1, &hitFN);
		hitFN._this = this;
		//healthComponent->SetUIYPos(sprite->GetHeight());
		//healthComponent->GetHPBarObject()->SetEnable(false);

		AddComponent<Selectable>();
		BoxCollider* collider = AddComponent<BoxCollider>();
		collider->SetSize(sprite->GetWidth() * 0.75f, sprite->GetHeight());
		collider->SetAnchor(0.0f, sprite->GetHeight() * 0.5f);


		//AddComponent<Highlightable_Chandelier>();
		SetDrawingPriority(OrderLayer::Chandelier);

		heartBackGround = AddObject<Object>();
		SpriteRenderer* sprite = heartBackGround->AddComponent<SpriteRenderer>();

		sprite->SetImage("TrainHeart/BackGround");
		sprite->SetSizeAuto();
		sprite->anchorType = AnchorType::BottomCenter;
		heartBackGround->SetDrawingPriority(OrderLayer::Chandelier - 1);

		heartBackGround->transform->SetParent(transform);
		heartBackGround->transform->SetPosition(30.0f, -20.0f, 0.0f);


		Lever = AddObject<Object>("Lever");
		Lever->SetDrawingPriority(OrderLayer::Player - 1);
		Lever->transform->SetPosition(-101.0f, 21.0f, 0.0f);
		SpriteRenderer* sprite2 = Lever->AddComponent<SpriteRenderer>();

		sprite2->SetImage("Train/Lever1");
		sprite2->SetSize(1800.0f, 900.0f);
	}
	void Start() override
	{
		gameManager->GetTile(currentTileindex - 1)->SetEnable(false);
		gameManager->GetTile(currentTileindex)->SetEnable(false);
		Lever->transform->SetParent(gameManager->transform);


		const PlayerText& text17 = PlayerTextManager().GetText(17);
		const PlayerText& text18 = PlayerTextManager().GetText(18);
		const PlayerText& text19 = PlayerTextManager().GetText(19);

		gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->BubbleDialog(text17.BubbleText, text17.FontSize, text17.BubbleDuration, true, Vector3(30.f, 0, 0));


		AddTimerFunction([this, text18](bool lastupdate) mutable
						 {
							 if (lastupdate)
							 {
								 gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->BubbleDialog(text18.BubbleText, text18.FontSize, text18.BubbleDuration, true, Vector3(10.f, 0, 0));
							 }
						 }, false, text17.BubbleDuration, 1);

		AddTimerFunction([this, text19](bool lastupdate) mutable
						 {
							 if (lastupdate)
							 {
								 gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->BubbleDialog(text19.BubbleText, text19.FontSize, text19.BubbleDuration, true, Vector3(-10.f, 0, 0));
							 }
						 }, false, text17.BubbleDuration + text18.BubbleDuration, 1);
	}
	void OnDestroy() override
	{
		heatBeat_channel->stop();
	}
	void Update() override
	{

	}

	FMOD::Channel* heatBeat_channel;
	Animator* animator;
	HealthComponent* healthComponent;
	SpriteRenderer* sprite;
	Object* heartBackGround;
	Object* Lever;

	HitFN hitFN;
};

GameObject* GameManager::AddMonsterInTileByID(int monsterID, int index, int weight)
{
	GameObject* newObject = nullptr;
	if (tileObjects[index]->object != nullptr)
	{
		if (weight == 9999) {
			return newObject;
		}

		if (tileObjects[index]->object->type == EType::Enemy && tileObjects[index]->object->GetComponent<HealthComponent>()->GetHp() > 1)
		{
			AudioFramework::Play_BankSound("Sound", "SFX_SPAWNFIRE_HIT");
		}

		currentKillMonsterCount++;
		DamageInTile(index, 1);
	}
	else if (monsterID == 1)
	{
		newObject = AddMonsterInTile<RushTest>(index, weight);
	}
	else if (monsterID == 2)
	{
		newObject = AddMonsterInTile<TestMonster>(index, weight);
	}
	else if (monsterID == 3)
	{
		newObject = AddMonsterInTile<FarMonster>(index, weight);
	}
	else if (monsterID == 100)
	{
		bossMonster = AddObject<Boss>();
		bossMonster->targetPlayer = player;
		bossMonster->gameManager = this;
		stageLockObject = bossMonster;
	}
	else if (monsterID == 200)
	{
		stageLockObject = AddGameObjectInTile<TrainHeart>(index);
	}

	return newObject;
}


template<>
Player* GameManager::AddGameObjectInTile<Player>(int index, Player* newObject)
{
	if (!newObject)
	{
		newObject = AddObject<Player>();
	}
	newObject->SetPositionBasedTile(GetTilePosition(index));
	newObject->gameManager = this;
	newObject->currentTileindex = index;
	tileObjects[index]->object = newObject;

	return newObject;
}

template <>
TestMonster* GameManager::AddMonsterInTile(int index, int weight)
{
	TestMonster* newObject = AddGameObjectInTile<TestMonster>(index);

	if (MonsterComponent* monsterComponent = newObject->GetComponent< MonsterComponent>(); monsterComponent)
	{
		monsterComponent->SetTargetPlayer(player);
		monsterComponent->SetEnemyInfo(3, 1, 1, 1, EnemyType::Near, currentTurn * 10 + weight);
		monsterObjects.emplace_back(newObject);
	}
	return newObject;
}

template <>
RushTest* GameManager::AddMonsterInTile(int index, int weight)
{
	RushTest* newObject = AddGameObjectInTile<RushTest>(index);

	if (MonsterComponent* monsterComponent = newObject->GetComponent< MonsterComponent>(); monsterComponent)
	{
		monsterComponent->SetTargetPlayer(player);
		monsterComponent->SetEnemyInfo(2, 2, 7, 1, EnemyType::Rush, currentTurn * 10 + weight);
		monsterObjects.emplace_back(newObject);
	}
	return newObject;
}

template <>
FarMonster* GameManager::AddMonsterInTile(int index, int weight)
{
	FarMonster* newObject = AddGameObjectInTile<FarMonster>(index);

	if (MonsterComponent* monsterComponent = newObject->GetComponent< MonsterComponent>(); monsterComponent)
	{
		monsterComponent->SetTargetPlayer(player);
		monsterComponent->SetEnemyInfo(2, 1, 2, 1, EnemyType::Far, currentTurn * 10 + weight);
		monsterObjects.emplace_back(newObject);
	}
	return newObject;
}

template<>
TestMonster* GameManager::AddGameObjectInTile<TestMonster>(int index, TestMonster* newObject)
{
	if (!newObject)
	{
		newObject = AddObject<TestMonster>();
	}
	newObject->SetPositionBasedTile(GetTilePosition(index));
	newObject->gameManager = this;
	newObject->currentTileindex = index;
	tileObjects[index]->object = newObject;

	if (MonsterComponent* monsterComponent = newObject->GetComponent< MonsterComponent>(); monsterComponent)
	{
		monsterComponent->SetTargetPlayer(player);
		monsterComponent->SetEnemyInfo(3, 1, 1, 1, EnemyType::Near, currentTurn * 2);
		monsterObjects.emplace_back(newObject);
	}
	return newObject;
}

template<>
RushTest* GameManager::AddGameObjectInTile<RushTest>(int index, RushTest* newObject)
{
	if (!newObject)
	{
		newObject = AddObject<RushTest>();
	}
	newObject->SetPositionBasedTile(GetTilePosition(index));
	newObject->gameManager = this;
	newObject->currentTileindex = index;
	tileObjects[index]->object = newObject;

	if (MonsterComponent* monsterComponent = newObject->GetComponent< MonsterComponent>(); monsterComponent)
	{
		monsterComponent->SetTargetPlayer(player);
		monsterComponent->SetEnemyInfo(3, 1, 7, 1, EnemyType::Rush, currentTurn * 3);
		monsterObjects.emplace_back(newObject);
	}
	return newObject;
}

template<>
FarMonster* GameManager::AddGameObjectInTile<FarMonster>(int index, FarMonster* newObject) 
{
	if (!newObject)
	{
		newObject = AddObject<FarMonster>();
	}
	newObject->SetPositionBasedTile(GetTilePosition(index));
	newObject->gameManager = this;
	newObject->currentTileindex = index;
	tileObjects[index]->object = newObject;

	if (MonsterComponent* monsterComponent = newObject->GetComponent< MonsterComponent>(); monsterComponent)
	{
		monsterComponent->SetTargetPlayer(player);
		monsterComponent->SetEnemyInfo(2, 1, 2, 1, EnemyType::Far, currentTurn * 1);
		monsterObjects.emplace_back(newObject);
	}
	return newObject;
}

template<>
BossLeftHand* GameManager::AddGameObjectInTile<BossLeftHand>(int index, BossLeftHand* newObject) {
	if (!newObject)
	{
		newObject = AddObject<BossLeftHand>();
	}
	newObject->SetPositionBasedTile(GetTilePosition(index));
	newObject->gameManager = this;
	newObject->currentTileindex = index;
	tileObjects[index]->object = newObject;

	if (MonsterComponent* monsterComponent = newObject->GetComponent< MonsterComponent>(); monsterComponent)
	{
		monsterComponent->SetTargetPlayer(player);
		monsterComponent->SetEnemyInfo(3, 100, 1, 0, EnemyType::Boss_Left, currentTurn * 4);
		monsterObjects.emplace_back(newObject);
	}
	return newObject;
}

template<>
BossRightHand* GameManager::AddGameObjectInTile<BossRightHand>(int index, BossRightHand* newObject) {
	if (!newObject)
	{
		newObject = AddObject<BossRightHand>();
	}
	newObject->SetPositionBasedTile(GetTilePosition(index));
	newObject->gameManager = this;
	newObject->currentTileindex = index;
	tileObjects[index]->object = newObject;

	if (MonsterComponent* monsterComponent = newObject->GetComponent< MonsterComponent>(); monsterComponent)
	{
		monsterComponent->SetTargetPlayer(player);
		monsterComponent->SetEnemyInfo(3, 100, 1, 0, EnemyType::Boss_Right, currentTurn * 4);
		monsterObjects.emplace_back(newObject);
	}
	return newObject;
}

template<>
Chandelier* GameManager::AddGameObjectInTile<Chandelier>(int index, Chandelier* newObject)
{
	if (!newObject)
	{
		newObject = AddObject<Chandelier>();
	}

	tileObjects[index]->chandelier = newObject;
	newObject->gameManager = this;
	newObject->currentTileindex = index;
	newObject->SetPositionBasedTile(GetTilePosition(index));
	return newObject;
}

template<>
GunPowder* GameManager::AddGameObjectInTile<GunPowder>(int index, GunPowder* newObject)
{
	if (!newObject)
	{
		newObject = AddObject<GunPowder>();
	}

	tileObjects[index]->gunPowder = newObject;
	newObject->gameManager = this;
	newObject->currentTileindex = index;
	newObject->SetPositionBasedTile(GetTilePosition(index));
	return newObject;
}


















class GameFlow_MonsterSpawnActionNode : public BT::ActionNode
{
#define GameFlow_MonsterSpawnActionNodeReflect(x)
	GENERATED_BODY(GameFlow_MonsterSpawnActionNode, BT::ActionNode)
public:

	virtual void OnEnter() override
	{
		owner = GetOwner<GameManager>();
		owner->NextTurn();
		elapsedTime = 0.0f;
	}
	virtual void OnExit() override
	{
		owner->NextPhase();
		owner->SpawnMonsterSpawner();
	}

	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		float SpawnTime = GetConfig().GameManager_MonsterSpawnTime;
		auto pair = owner->monsterSpawners.equal_range(owner->currentTurn);

		if (owner->bossMonster != nullptr)
		{
			if (owner->isBossStageReady == true) return BT::Status::Success;
			else if (owner->isBossStageReady == false) return BT::Status::Running;
		}


		if (pair.first == pair.second)
		{
			return BT::Status::Success;
		}
		elapsedTime += Time::GetDeltaTime();
		
		if (elapsedTime > SpawnTime)
		{
			elapsedTime -= SpawnTime;
			auto minIter = pair.first;
			for (auto iter = minIter; iter != pair.second; iter++)
			{
				if (iter->second->spawnID <= minIter->second->spawnID &&
					iter->second->weight <= minIter->second->weight)
				{
					minIter = iter;
				}
			}

			minIter->second->Spawn();
			owner->monsterSpawners.erase(minIter);
		}


		return BT::Status::Running;
	}


	float elapsedTime;
	
	GameManager* owner;
};








class GameFlow_DecideEnemyActionNode : public BT::ActionNode
{
#define GameFlow_DecideEnemyActionNodeReflect(x)
	GENERATED_BODY(GameFlow_DecideEnemyActionNode, BT::ActionNode)
public:

	virtual void OnEnter() override
	{
		owner = GetOwner<GameManager>();

		static std::random_device rd;
		static std::mt19937 gen(rd());


		if (owner->GetLeftTurn() == 1)
		{
			static std::uniform_int_distribution<int> dis(8, 10);
			const PlayerText& text = PlayerTextManager().GetText(dis(gen));
			owner->GetPlayer()->GetComponent<ChatBalloonComponent>()->BubbleDialog(text.BubbleText, text.FontSize, text.BubbleDuration);
		}
		else
		{
		}
		count = 0;
	}
	virtual void OnExit() override
	{
		owner->UpdateIndicator();


		owner->NextPhase();
	}

	virtual BT::Status::Type OnBehave(float deltaTime) override 
	{
		//if (count++ > 1)
		if (count++ > 2)
		{
			return BT::Status::Success;
		}
		else
		{
			return BT::Status::Running;
		}
	}
	int count;
	GameManager* owner;
};



class GameFlow_PlayerActionNode : public BT::ActionNode
{
#define GameFlow_PlayerActionNodeReflect(x)
	GENERATED_BODY(GameFlow_PlayerActionNode, BT::ActionNode)
public:
	virtual void OnEnter() override
	{
		elapseTime = 0.0f;

	}
	virtual void OnExit() override
	{

	}

	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		GameManager* owner = GetOwner<GameManager>();
		
		elapseTime += Time::GetDeltaTime();
		if (elapseTime > GetConfig().GameManager_AFKTextTime)
		{
			elapseTime = 0.0f;

			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_int_distribution<int> dis(5, 7);

			int randomNum = dis(gen);

			const PlayerText& text = PlayerTextManager().GetText(randomNum);

			if (randomNum == 6) {
				owner->GetPlayer()->GetComponent<ChatBalloonComponent>()->BubbleDialog(text.BubbleText, text.FontSize, text.BubbleDuration, false, Vector3(-10.f, 0, 0));
			}
			else if (randomNum == 7) {
				owner->GetPlayer()->GetComponent<ChatBalloonComponent>()->BubbleDialog(text.BubbleText, text.FontSize, text.BubbleDuration, false, Vector3(10.f, 0, 0));
			}
			else {
				owner->GetPlayer()->GetComponent<ChatBalloonComponent>()->BubbleDialog(text.BubbleText, text.FontSize, text.BubbleDuration);
			}
		}
		if (owner->GetCurrentPhase() == Phase::Player_Action)
		{
			return BT::Status::Running;
		}
		else
		{
			for (auto monster : owner->monsterObjects)
			{
				if (monster == nullptr) continue;
				monster->GetComponent<PrintActionPriority>()->HidePriority();
			}


			return BT::Status::Success;
		}
	}

	float elapseTime;
};

class GameFlow_ProcessCommandNode : public BT::ActionNode
{
#define GameFlow_ProcessCommandNodeReflect(x)
	GENERATED_BODY(GameFlow_ProcessCommandNode, BT::ActionNode)
public:
	virtual void OnEnter() override
	{
		owner = GetOwner<GameManager>();

		owner->commandWaitTime = GetConfig().GameManager_CommandWaitTime;
	}
	virtual void OnExit() override
	{
		owner->NextPhase();

		owner->ClearCommandList();

	}

	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		if (owner->ProcessCommandList())
		{
			return BT::Status::Running;
		}
		else
		{
			return BT::Status::Success;
		}
	}
	GameManager* owner;
};

class GameFlow_IsStageClearNode : public BT::ActionNode
{
#define GameFlow_IsStageClearNodeReflect(x)
	GENERATED_BODY(GameFlow_IsStageClearNode, BT::ActionNode)
public:
	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		GameManager* owner;
		owner = GetOwner<GameManager>();

		bool isMonsterTerminated = owner->monsterObjects.size() == 0;
		bool isNotTurn0 = owner->GetCurrentTurn() > 0;
		bool isStageLock = owner->stageLockObject;
		if (isMonsterTerminated && isNotTurn0 && !isStageLock)
		{
			owner->ClearStage();
			return BT::Status::Success;
		}
		else
		{
			owner->NextPhase();
			return BT::Status::Failure;
		}
	}
};
class GameFlow_GameClearNode : public BT::ActionNode
{
#define GameFlow_GameClearNodeReflect(x)
	GENERATED_BODY(GameFlow_GameClearNode, BT::ActionNode)
public:
	virtual void OnEnter() override
	{
		elapsedTime = 0.0f;
	}

	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		GameManager* owner;
		owner = GetOwner<GameManager>();
		if (owner->isGameClear)
		{
			elapsedTime += Time::GetDeltaTime();

			if (elapsedTime > 1.0f)
			{

				Object* Lever = owner->GetOwner()->GetObjectByName<Object>("Lever");
				if (Lever)
				{
					Lever->SetDontDestroyOnSceneChange(true);
					Lever->transform->SetParent(nullptr);
				}
				owner->GetPlayer()->SetDontDestroyOnSceneChange(true);
				owner->maintrain->SetDontDestroyOnSceneChange(true);
				SceneManager::LoadScene<CreditScene>();
			}
			return BT::Status::Running;
		}
		else
		{
			owner->GetPlayer()->GetComponent<HealthComponent>()->ResetHp();

			return BT::Status::Success;
		}
	}
	float elapsedTime;
};


class GameFlow_StageStartNode : public BT::ActionNode
{
#define GameFlow_StageStartNodeReflect(x)
	GENERATED_BODY(GameFlow_StageStartNode, BT::ActionNode)
public:
	virtual void OnEnter() override
	{
		elapsedTime = 0;
		owner = GetOwner<GameManager>();

		owner->GetPlayer()->GetComponent<HealthComponent>()->GetHPBarObject()->SetEnable(false);

		for (auto& item : owner->tileObjects)
		{
			item->SetEnable(false);
		}

		if (!owner->tempLeftTrain)
		{
			owner->tempLeftTrain = owner->AddObject<Object>();
			SpriteRenderer* sprite = owner->tempLeftTrain->AddComponent<SpriteRenderer>();
			sprite->SetImage("Train/Left");
			sprite->SetSizeAuto();
			owner->tempLeftTrain->SetDrawingPriority(OrderLayer::TrainWheel);
			owner->tempLeftTrain->transform->SetParent(Camera::GetMainCamera()->GetTransform());
		}
		
		if (!owner->tempRightTrain)
		{
			owner->tempRightTrain = owner->AddObject<Object>();
			SpriteRenderer* sprite = owner->tempRightTrain->AddComponent<SpriteRenderer>();
			sprite->SetImage("Train/Right");
			sprite->SetSizeAuto();
			owner->tempRightTrain->SetDrawingPriority(OrderLayer::TrainWheel);
			owner->tempRightTrain->transform->SetParent(Camera::GetMainCamera()->GetTransform());
		}



		if (owner->oldtrain)
		{
			owner->oldtrain->WaitAndOutOut();
			owner->oldtrain = nullptr;
		}

		if (owner->maintrain)
		{
			owner->maintrain->CurtainColseAndOut();
			owner->oldtrain = owner->maintrain;
			owner->maintrain = nullptr;
		}

		if (!owner->newtrain)
		{
			owner->newtrain = owner->AddObject<TrainSpriteObject>();;
			owner->newtrain->Init(owner->GetTrain(owner->GetCurrentStage()), owner->GetBackGround(owner->GetCurrentStage()));
			owner->newtrain->ComeCome();

			elapsedTime = -0.7f;
		}

		owner->maintrain = owner->newtrain;
		owner->newtrain = nullptr;
		owner->AddTimerFunction([this](bool lastUpdate) mutable
								{
									if (lastUpdate)
									{
										owner->maintrain->WaitAndCome();
									}
								}, false, -elapsedTime, 1);



		owner->newtrain = owner->AddObject<TrainSpriteObject>();;
		owner->newtrain->Init(owner->GetTrain(owner->GetCurrentStage() + 1), owner->GetBackGround(owner->GetCurrentStage() + 1));
		owner->newtrain->WaitAndComeCome();

		owner->tempLeftTrain->SetEnable(false);
		owner->tempRightTrain->SetEnable(false);

		AudioFramework::Play_BankSound("Sound", "SFX_Train_Door");

		Player* player = owner->GetPlayer();
		player->transform->SetPositionY(DEFAULT_TILE_Y);

		owner->AddTimerFunction([player, this](bool lastUpdate) mutable
								{
									if (lastUpdate)
									{
										/*owner->tileObjects[3]->object = player;
										player->SetCurrentTileIndex(3);
										player->moveTargetTileIndex = 3;*/
										player->MoveToTile({ .tileIndex = 3, .easing = &Math::Lerp,.duration = 5.0f,   .isSwap = true });
									}
								}, false, -elapsedTime, 1);

		if (player->IsFaceRight() == false) player->FlipX();

		auto anim_Run = ResourceManager::GetAnimation("player/Run");
		anim_Run.get()->loop = true;
		player->animator->SetAnimation(*anim_Run);
		player->animator->enable = true;
		player->renderer->anchor = Vector2{ 0, 0 };
		player->renderer->SetSizeAuto();

		float elapsedTime = 0.0f;
		player->AddTimerFunction([player, elapsedTime](bool lastUpdate) mutable
											 {
												 if (!lastUpdate)
												 {
													 player->animator->enable = true;
													 elapsedTime += Time::GetDeltaTime() / 2.5f;
													 player->animator->SetInterval(Math::Lerp(0.1f, 0.02f, 1.0f - std::abs(1.0f - elapsedTime)));
												 }
												 else
												 {
													player->animator->SetAnimation(*ResourceManager::GetAnimation("player/idle"));
													player->UpdateSprite("player/idle");

													player->animator->enable = true;
													player->renderer->SetSizeAuto();
												 }
											 }, true, 5.0f, 1);

	}
	virtual void OnExit() override
	{
		owner->GetPlayer()->transform->SetParent(nullptr);
		owner->NextStage();
		owner->GetPlayer()->GetComponent<HealthComponent>()->GetHPBarObject()->SetEnable(true);


		for (auto& item : owner->tileObjects)
		{
			item->SetEnable(true);
		}
		owner->tempRightTrain->SetEnable(false);
		if (owner->GetCurrentStage() > 1)
		{
			owner->tempLeftTrain->SetEnable(false);
		}

		owner->maintrain->Default();
		owner->NextPhase();

	}

	virtual BT::Status::Type OnBehave(float deltaTime) override
	{
		elapsedTime += Time::GetDeltaTime();

		if (elapsedTime > 5.0f)
		{
			return BT::Status::Success;
		}
		Camera::GetMainCamera()->GetTransform()->SetPositionX(0.0f);

		return BT::Status::Running;
	}
	GameManager* owner;



	float elapsedTime;
};


