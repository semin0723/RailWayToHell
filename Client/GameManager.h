#pragma once
#include <vector>
#include <map>
#include "Engine.h"
#include "Tile.h"
#include "SampleScene.h"
#include "GameObject.h"
#include "StageInfo.h"
#include "TurnPannel.h"
#include "NextWaveInfoPannel.h"
#include "StageGoalPannel.h"


class StageInfo;
class StageInfo2;
class IStageInfo;

class Cursor_Aim;
class HighlightManager;
struct ICommand;

class Player;
class TestMonster;
class RushTest;
class FarMonster;
class Trap;
class GunPowder;
class Chandelier;
class BossLeftHand;
class BossRightHand;

class MonsterSpawner;
class TrainSpriteObject;

// 1. Decide_Enemy_Action : 현재 타일 상태를 기반으로 적들의 다음 행동을 결정
// 2. Player_Action : 플레이어의 행동(상호작용, 상호작용의 결과 모두 여기서 처리)
// 3. Enemy_Action : 1에서 결정된 행동을 수행 / 예외적으로 이동할 때 플레이어가 앞에 끼어든 경우에만 공격으로 변경
namespace Phase
{ 
	enum Type
	{
		StageClear,
		MonsterSpawn,
		CheckStageClear,
		Decide_Enemy_Action,
		Player_Action, 
		ProcessCommand,
		MAX
	};
};

struct ActSpeed
{
	static inline constexpr int PlayerMove = INT_MIN;
	static inline constexpr int PlayerAttack = PlayerMove + 1;
	static inline constexpr int InteractableObject = PlayerAttack + 1;
};

class GameManager : public Object
{
	/** csv로 역직렬화된 스테이지 정보 */
	std::vector<StageInfo> StageInfosOriginal;
	std::vector<StageInfo2*> StageInfos2Original;
	std::map<int, IStageInfo*> stageInfos;
	std::map<int, TrainInfo> trainInfo;
	

	friend class GameFlow_MonsterSpawnActionNode;
	int currentStage;

	/** yaml로 역직렬화된 스테이지 정보 */
	std::map<int, StructureInfo> StructureInfos;

	/** 스테이지 정보 컨테이너의 주소를 가르키는 현재 스테이지 정보 */
	IStageInfo* currentStageInfo;

	/** 현재턴입니다. */
	int currentTurn;

	/** 등록된 상호작용 오브젝트 입니다. */
	GameObject* interactionObject;
	BT::BehaviourTreeV2* gameFlowBT;

	Phase::Type currentPhase;
	using CommandList = std::multimap<int, ICommand*>;
	CommandList commandList;
	CommandList::iterator currentCommand;
	CommandList prevcommandList;

	int prevPlayerIndex;
	int playerIndex;	
	static inline const int playerInitIndex = TILE_CENTER_INDEX;

	
	Player* player;

	/** 현재 스테이지의 총 마릿수와 현재까지 처치한 마릿수를 나타내는 변수들 입니다. */
	// 현재 스테이지에 나오는 몬스터의 총 마릿수 입니다.
	// 스테이지에 들어올 때 데이터에서 읽어 여기에 넣어줘야 합니다.
	int totalStageMonsterCount = 0;

	// 소환에 실패했을 시에도 증가해야 합니다.
	// 스테이지 진입 시 초기화 돼야 합니다.
	int currentKillMonsterCount = 0;
public:
	class SoundBankChannel* backGroundMsusic;
	std::multimap<int, MonsterSpawner*> monsterSpawners;

	bool isGameClear;
	bool isGameOver;
	bool is_developer_mode_on = true;
	bool isBossStageReady;

	GameObject* stageLockObject; // 보스 본체
	class Boss* bossMonster; // 이것도 보스 본체 ㅗㅗㅗㅗ

	bool IsGameEnd() { return isGameClear || isGameOver; }
	std::vector<TileObject*> tileObjects;
	std::vector<GameObject*> monsterObjects;
	static inline constexpr int InitTileSize = 7;

	// Turn info UI	
	NextWaveInfoPannel* waveInfoUI;
	StageGoalPannel* stageGoalUI;

	// 샹들리에
	// 중간에 껴있는 오브젝트
	// 그 외 기타 등등
	Cursor_Aim* cursor_aim;
	Object* shade;
	HighlightManager* highlightManager;
	class PauseMenu* pauseMenu;
	float commandWaitTime;

	TrainSpriteObject* newtrain;
	TrainSpriteObject* maintrain;
	TrainSpriteObject* oldtrain;
	Object* tempLeftTrain;
	Object* tempRightTrain;
	

	virtual void Awake() override;
	virtual void OnDestroy() override;
	virtual void Start() override;
	virtual void FixedUpdate() override;
	virtual void Update() override;

	void SetCurrentPhase(Phase::Type phase) { currentPhase = phase; }
	Phase::Type GetCurrentPhase() { return currentPhase; }
	void ClearStage();
	/** 다음 스테이지로 이동합니다. */
	void NextStage();
	/** 현재 스테이지의 모든 적들을 제거합니다. */
	void KillThemAll();
	
	/** 다음 턴으로 이동합니다. */
	void NextTurn();
	void NextPhase();

	void GameOver();
	void GameClear();

	/** 파일이름으로 스테이지 정보를 읽어옵니다 ** 현재는 csv만 됨 ** */
	void LoadStageFile(const char* filename = "");

	// 이제 씬을 초기화할 때를 제외하고 이 함수를 쓰는 일은 없음
	void Restart();
	// 씬을 재로드할 필요가 있을 때 이 놈을 호출함
	void PrepareRestart();


	void SkipTutorial();
	bool CanMove(int index);

	Vector3 GetTilePosition(int tileIndex);
	void SetInteractionObject(GameObject* interactionObject) { this->interactionObject = interactionObject; }

	void RegisterCommandList(int actSpeed, ICommand* command);
	void UnRegisterCommandList(ICommand* command);


	Player* GetPlayer() { return player; }

	/**
	 * @brief 플레이어의 동작 처리
	 * @param prevIndex 이동전 인덱스
	 * @param targetIndex 이동후 인덱스
	 * @param isSwap 변경여부
	 * @return 이동 성공여부
	 */
	bool ObjectMove(int prevIndex, int targetIndex, bool isSwap = false);
	int GetObjectIndex(GameObject* gamObject);
	TileObject* GetTile(int index) { if (!IsTileRangeIndex(index)) return nullptr;  return tileObjects[index]; }
	int GetTrapIndex(Trap* trap);
	int GetLeftTurn();
	int GetCurrentTurn() { return currentTurn; }
	void SetCurrentStage(int stage) { currentStage = stage; }
	int GetCurrentStage() { return currentStage; }
	bool IsFinalStage() { return !currentStageInfo; }
	void DamageInTile(int index, int damage);

	// 플레이어의 넉백탄 / 화약통 폭발시 사용됨
	void Knockback(bool triggeredFromPlayerAttack, GameObject* target, int attackDirection);

	bool IsTileRangeIndex(int index) { return index >= 0 ? index < tileObjects.size() : false; }

	template<class T>
	T* AddGameObjectInTile(int index, T* newObject = nullptr);

	template<class T>
	T* AddMonsterInTile(int index, int weight = 0);

	GameObject* AddMonsterInTileByID(int monsterID, int index, int weight = 0);
	void DestroyObjectInTile(GameObject* destroyObject);

	std::string GetTrain(int index);
	std::string GetBackGround(int index);
	MonsterSpawner* GetSpawner(int index);

	void SpawnMonsterInTile(int targetIndex, int monsterId, int actWeight);
	void IncreseKillCount() { currentKillMonsterCount++; }
public:
	void ClearCommandList();
	bool ProcessCommandList();

	void OnCharacterDie(GameObject* subject);




	// Indicator
	void UpdateIndicator();
private:
	void SpawnMonsterSpawner();
};

template<>
TestMonster* GameManager::AddMonsterInTile(int index, int weight);
template<>
RushTest* GameManager::AddMonsterInTile(int index, int weight);
template<>
FarMonster* GameManager::AddMonsterInTile(int index, int weight);

template<>
Player* GameManager::AddGameObjectInTile<Player>(int index, Player* newObject);
template<>
TestMonster* GameManager::AddGameObjectInTile<TestMonster>(int index, TestMonster* newObject);
template<>
RushTest* GameManager::AddGameObjectInTile<RushTest>(int index, RushTest* newObject);
template<>
FarMonster* GameManager::AddGameObjectInTile<FarMonster>(int index, FarMonster* newObject);
template<>
BossLeftHand* GameManager::AddGameObjectInTile<BossLeftHand>(int index, BossLeftHand* newObject);
template<>
BossRightHand* GameManager::AddGameObjectInTile<BossRightHand>(int index, BossRightHand* newObject);
template<>
Chandelier* GameManager::AddGameObjectInTile<Chandelier>(int index, Chandelier* newObject);
template<>
GunPowder* GameManager::AddGameObjectInTile<GunPowder>(int index, GunPowder* newObject);

template<class T>
inline T* GameManager::AddGameObjectInTile(int index, T* newObject)
{
	if (!newObject)
	{
		newObject = AddObject<T>();
	}
	newObject->SetPositionBasedTile(GetTilePosition(index));
	newObject->gameManager = this;
	newObject->currentTileindex = index;
	tileObjects[index]->object = newObject;

	return newObject;
}
