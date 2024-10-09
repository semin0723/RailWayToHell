#pragma once
// �÷��̾�, ��, ��ȣ�ۿ� ������ ������Ʈ ���̽�

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

//Ÿ�Ͽ� ���ӵ� ������Ʈ
class GameObject : public Object
{
public:
	void SetPositionBasedTile(Vector3 position);
	void UnRegisterGameManager();

	EType type{ EType::Default };


	// �ش� Ÿ�Ϸ� �̵��ϴ� �ִϸ��̼ǰ� �Բ� currentTileIndex ����
	void MoveToTile(MoveToArg arg);
	void MoveToPosition(MoveToArg arg);
	// �ش� Ÿ�Ϸ� �̵����� �ʰ� ƨ�ܳ����� �ִϸ��̼Ǹ� ����
	void KnockbackBounce(int collideTile, int bouncedTile, float firstMoveDuration, float secondMoveDuration);
	// �ǰ� � ��¦ �и��� �ִϸ��̼�
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

