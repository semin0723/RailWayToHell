#include "GameObject.h"
#include "defines.h"
#include "GameManager.h"




void GameObject::SetPositionBasedTile(Vector3 position)
{
	transform->Translate(-transform->GetPosition() + position + RelativePosition);
}

void GameObject::UnRegisterGameManager()
{
	gameManager->DestroyObjectInTile(this);
}

void GameObject::MoveToTile(MoveToArg arg)
{
	if (!DEFINE::IsRangeTileIndex(arg.tileIndex))
	{
		(*arg.isFinish) = true;
		return;
	}
	float initX = transform->GetPosition().x;
	float targetX = DEFINE::GetTilePositionX(arg.tileIndex);
	float t = 0.0f;
	if(!arg.ignoreObject) gameManager->ObjectMove(currentTileindex, arg.tileIndex, arg.isSwap);
	AddTimerFunction([=](bool lu) mutable
		{
			if (!lu)
			{
				t += Time::GetDeltaTime() / arg.duration;
				transform->SetPositionX(arg.easing(initX, targetX, t));
			}
			else
			{
				transform->SetPositionX(targetX);
				if (arg.isFinish) {
					*(arg.isFinish) = true;
				}
			}
		}, true, arg.duration, 1);
}

void GameObject::MoveToPosition(MoveToArg arg)
{
	if (!DEFINE::IsRangeTileIndex(arg.tileIndex))
	{
		(*arg.isFinish) = true;
		return;
	}
	Vector3 initPosition = transform->GetPosition();
	Vector3 targetPos = gameManager->GetTilePosition(arg.tileIndex) + arg.RelativePosition;
	float t = 0.0f;
	gameManager->ObjectMove(currentTileindex, arg.tileIndex, arg.isSwap);

	AddTimerFunction([=](bool lu) mutable
					 {
						 if (!lu)
						 {
							 t += Time::GetDeltaTime() / arg.duration;
							 transform->SetPosition({ arg.easing(initPosition.x, targetPos.x, t), arg.easing(initPosition.y, targetPos.y, t), 0.0f });
						 }
						 else
						 {
							 transform->SetPosition(targetPos);
						 }
					 }, true, arg.duration, 1);
}




void GameObject::KnockbackBounce(int collideTile, int bouncedTile, float firstMoveDuration, float secondMoveDuration)
{
	float firstX;
	float finalX;


	// 벽이랑 충돌할 때
	if (collideTile == -1)
	{
		firstX = DEFINE::GetTilePositionX(0);
		firstX += -100.f;
	}
	else if (collideTile == GameManager::InitTileSize)
	{
		firstX = DEFINE::GetTilePositionX(GameManager::InitTileSize - 1);
		firstX += 100.f;
	}
	// 다른 적과 충돌했을 때
	else
	{
		float x = DEFINE::GetTilePositionX(collideTile);
		float y = DEFINE::GetTilePositionX(bouncedTile);
		firstX = (DEFINE::GetTilePositionX(collideTile) + DEFINE::GetTilePositionX(bouncedTile)) / 2.0f;
	}
	finalX = DEFINE::GetTilePositionX(bouncedTile);

	float initPos = transform->GetPosition().x;
	float t = 0.0f;

	AddTimerFunction([=](bool lu) mutable
		{
			t += Time::GetDeltaTime() / firstMoveDuration;
			if (t > 1.0f) t = 1.0f;

			transform->SetPositionX(Math::Lerp(initPos, firstX, t));

			if (lu)
			{
				t = 0.0f;
				initPos = this->transform->GetPosition().x;
				AddTimerFunction([=](bool lu) mutable
					{
						t += Time::GetDeltaTime() / secondMoveDuration;
						if (t > 1.0f) t = 1.0f;
						this->transform->SetPositionX(Math::EaseOutCubic(initPos, finalX, t));
					}, true, secondMoveDuration, 1);
			}
		}, true, firstMoveDuration, 1);




	//if (!DEFINE::IsRangeTileIndex(arg.tileIndex))
	//{
	//	(*arg.isFinish) = true;
	//	return;
	//}
	//float initX = transform->GetPosition().x;
	//float targetX = DEFINE::GetTilePositionX(arg.tileIndex);
	//float t = 0.0f;

	//bool foo = false;

	//gameManager->ObjectMove(currentTileindex, arg.tileIndex, arg.isSwap);
	//AddTimerFunction([=](bool lu) mutable
	//	{
	//		//if (foo == true) return;
	//		if (!lu)
	//		{
	//			t += Time::GetDeltaTime() / arg.duration;
	//			transform->SetPositionX(arg.easing(initX, targetX, t));
	//		}
	//		else
	//		{
	//			targetX = DEFINE::GetTilePositionX(arg.tileIndex - arg.dir);
	//			transform->SetPositionX(targetX);
	//			if (arg.isFinish) {
	//				*(arg.isFinish) = true;
	//			}
	//		}

	//		//t += Time::GetDeltaTime() / arg.duration * 2;
	//		//if (t <= 1.f)
	//		//{
	//		//	transform->SetPositionX(Math::Lerp(initX, targetX, t));
	//		//}
	//		//else
	//		//{
	//		//	initX = transform->GetPosition().x;
	//		//	targetX = DEFINE::GetTilePositionX(arg.tileIndex - (arg.dir / arg.dir));
	//		//	
	//		//	//AddTimerFunction([=](bool lu) mutable
	//		//	//	{
	//		//	//		if (!lu)
	//		//	//		{
	//		//	//			t += Time::GetDeltaTime() / arg.duration;
	//		//	//			transform->SetPositionX(Math::Lerp(initX, targetX, t));
	//		//	//		}
	//		//	//		else
	//		//	//		{
	//		//	//			transform->SetPositionX(targetX);
	//		//	//			if (arg.isFinish) {
	//		//	//				*(arg.isFinish) = true;
	//		//	//			}
	//		//	//		}
	//		//	//	}, true, 0.3f, 1);
	//		//}
	//	}, true, arg.duration, 1);
}

void GameObject::PlayHitKnockbackMotion(int moveDir, float moveAmount, float duration)
{
	float oriX = transform->GetPosition().x;
	float pushX = oriX + (moveDir * moveAmount);

	float t = 0.0f;
	AddTimerFunction([=](bool lu) mutable
		{
			t += Time::GetDeltaTime() / duration * 0.5f;
			if (t > 1.0f) t = 1.0f;

			transform->SetPositionX(Math::EaseOutCubic(oriX, pushX, t));
			if (lu)
			{
				t = 0.0f;
				AddTimerFunction([=](bool lu) mutable
					{
						t += Time::GetDeltaTime() / duration * 0.5f;
						if (t > 1.0f) t = 1.0f;
						this->transform->SetPositionX(Math::EaseOutCubic(pushX, oriX, t));
					}, true, duration * 0.5f, 1);
			}
		}, true, duration * 0.5f, 1);
}

void GameObject::Die()
{
	if (gameManager == nullptr)
	{
		gameManager = GetOwner()->GetObjectByName<GameManager>("GameManager");
	}

	gameManager->OnCharacterDie(this);
}

bool GameObject::IsFaceRight()
{
	// 플레이어는 기본 오른쪽
	if (type == EType::Player)
	{
		return transform->GetScale().x > 0;
	}
	// 적은 기본 왼쪽
	else if (type == EType::Enemy)
	{
		return transform->GetScale().x < 0;
	}
}

void GameObject::FlipX()
{
	float targetX;
	if (transform->GetScale().x > 0) targetX = -1.f;
	else if (transform->GetScale().x < 0) targetX = 1.f;

	/*float t = 0.0f;

	AddTimerFunction([=](bool lu) mutable
		{
			t += Time::GetDeltaTime() / 0.5f;
			transform->SetScaleX(Math::EaseInOutCubic(transform->GetScale().x, targetX, t));
		}, true, 0.5f, 1);*/


	transform->SetScaleX(targetX);
}

void GameObject::FadeIn(float duration)
{
	SetEnable(true);
	GetComponent<SpriteRenderer>()->color.A = 0.0f;
	AddRealTimeTimerFunction([=](bool lu)
							 {
								 GetComponent<SpriteRenderer>()->color.A += Time::GetDeltaTime() / duration;
								 if (lu)
								 {
									 GetComponent<SpriteRenderer>()->color.A = 1.0f;
								 }
							 }, true, duration, 1);
}

void GameObject::FadeOut(float duration)
{
	SetEnable(true);
	GetComponent<SpriteRenderer>()->color.A = 1.0f;
	AddRealTimeTimerFunction([=](bool lu)
							 {
								 GetComponent<SpriteRenderer>()->color.A -= Time::GetDeltaTime() / duration;
								 if (lu)
								 {
									 Kill();
								 }
							 }, true, duration, 1);
}
