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

// 1. Decide_Enemy_Action : ���� Ÿ�� ���¸� ������� ������ ���� �ൿ�� ����
// 2. Player_Action : �÷��̾��� �ൿ(��ȣ�ۿ�, ��ȣ�ۿ��� ��� ��� ���⼭ ó��)
// 3. Enemy_Action : 1���� ������ �ൿ�� ���� / ���������� �̵��� �� �÷��̾ �տ� ����� ��쿡�� �������� ����
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
	/** csv�� ������ȭ�� �������� ���� */
	std::vector<StageInfo> StageInfosOriginal;
	std::vector<StageInfo2*> StageInfos2Original;
	std::map<int, IStageInfo*> stageInfos;
	std::map<int, TrainInfo> trainInfo;
	

	friend class GameFlow_MonsterSpawnActionNode;
	int currentStage;

	/** yaml�� ������ȭ�� �������� ���� */
	std::map<int, StructureInfo> StructureInfos;

	/** �������� ���� �����̳��� �ּҸ� ����Ű�� ���� �������� ���� */
	IStageInfo* currentStageInfo;

	/** �������Դϴ�. */
	int currentTurn;

	/** ��ϵ� ��ȣ�ۿ� ������Ʈ �Դϴ�. */
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

	/** ���� ���������� �� �������� ������� óġ�� �������� ��Ÿ���� ������ �Դϴ�. */
	// ���� ���������� ������ ������ �� ������ �Դϴ�.
	// ���������� ���� �� �����Ϳ��� �о� ���⿡ �־���� �մϴ�.
	int totalStageMonsterCount = 0;

	// ��ȯ�� �������� �ÿ��� �����ؾ� �մϴ�.
	// �������� ���� �� �ʱ�ȭ �ž� �մϴ�.
	int currentKillMonsterCount = 0;
public:
	class SoundBankChannel* backGroundMsusic;
	std::multimap<int, MonsterSpawner*> monsterSpawners;

	bool isGameClear;
	bool isGameOver;
	bool is_developer_mode_on = true;
	bool isBossStageReady;

	GameObject* stageLockObject; // ���� ��ü
	class Boss* bossMonster; // �̰͵� ���� ��ü �ǤǤǤ�

	bool IsGameEnd() { return isGameClear || isGameOver; }
	std::vector<TileObject*> tileObjects;
	std::vector<GameObject*> monsterObjects;
	static inline constexpr int InitTileSize = 7;

	// Turn info UI	
	NextWaveInfoPannel* waveInfoUI;
	StageGoalPannel* stageGoalUI;

	// ���鸮��
	// �߰��� ���ִ� ������Ʈ
	// �� �� ��Ÿ ���
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
	/** ���� ���������� �̵��մϴ�. */
	void NextStage();
	/** ���� ���������� ��� ������ �����մϴ�. */
	void KillThemAll();
	
	/** ���� ������ �̵��մϴ�. */
	void NextTurn();
	void NextPhase();

	void GameOver();
	void GameClear();

	/** �����̸����� �������� ������ �о�ɴϴ� ** ����� csv�� �� ** */
	void LoadStageFile(const char* filename = "");

	// ���� ���� �ʱ�ȭ�� ���� �����ϰ� �� �Լ��� ���� ���� ����
	void Restart();
	// ���� ��ε��� �ʿ䰡 ���� �� �� ���� ȣ����
	void PrepareRestart();


	void SkipTutorial();
	bool CanMove(int index);

	Vector3 GetTilePosition(int tileIndex);
	void SetInteractionObject(GameObject* interactionObject) { this->interactionObject = interactionObject; }

	void RegisterCommandList(int actSpeed, ICommand* command);
	void UnRegisterCommandList(ICommand* command);


	Player* GetPlayer() { return player; }

	/**
	 * @brief �÷��̾��� ���� ó��
	 * @param prevIndex �̵��� �ε���
	 * @param targetIndex �̵��� �ε���
	 * @param isSwap ���濩��
	 * @return �̵� ��������
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

	// �÷��̾��� �˹�ź / ȭ���� ���߽� ����
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
