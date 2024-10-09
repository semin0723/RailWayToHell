#pragma once
// 플레이어, 적, 상호작용 가능한 오브젝트 베이스

#include "Engine.h"
#include "Config.h"

class GameManager;

enum class EType { Default, Player, Enemy, Structure, Else };
using Easing = float (*)(float, float, float);

struct MoveToArg 
{
	int dir;
	int tileIndex;
	Vector3 RelativePosition;
	Easing easing = Math::EaseOutCubic;
	float duration = 0.4f;
	bool ignoreObject = false;
	bool isSwap = false;
	bool* isFinish = nullptr;
};

//타일에 종속된 오브젝트
class GameObject : public Object
{
public:
	void SetPositionBasedTile(Vector3 position);
	void UnRegisterGameManager();

	EType type{ EType::Default };


	// 해당 타일로 이동하는 애니메이션과 함께 currentTileIndex 변경
	void MoveToTile(MoveToArg arg);
	void MoveToPosition(MoveToArg arg);
	// 해당 타일로 이동하지 않고 튕겨나오는 애니메이션만 실행
	void KnockbackBounce(int collideTile, int bouncedTile, float firstMoveDuration, float secondMoveDuration);
	// 피격 등에 살짝 밀리는 애니메이션
	void PlayHitKnockbackMotion(int moveDir, float moveAmount, float duration);

	void Die();
	const int GetCurrentTileIndex() const { return currentTileindex; }
	void SetCurrentTileIndex(int index) { currentTileindex = index; }
	virtual void SetAnimation(std::string key) {}


	bool IsFaceRight();
	void FlipX();

	void FadeIn(float duration = 0.4f);


	void FadeOut(float duration = 0.4f);



protected:
	Vector3 RelativePosition;
	int currentTileindex;
	GameManager* gameManager;
	friend GameManager;
};

