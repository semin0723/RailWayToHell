#pragma once
#include "Engine.h"
#include "GameObject.h"
#include "Config.h"

class SceneEffect : public Object
{
public:
	bool damping = false;
	bool shakeTrain = true;
	float shakeTrainInterval = GetConfig().Effect_ShakeTrainInterval;
	float shakeTrainDuration = GetConfig().Effect_ShakeTrainDuration;
	float shakeTrainMagnitude = GetConfig().Effect_ShakeTrainMagnitude;
	float elapsedTime = 0.0f;

	float targetScale = 1.0f;
	Vector3 targetPos = { 0, 0, 0 };
	Camera* mainCam = nullptr;
	Object* CamObj = nullptr;

	float duration = 0.0f;
	static float fVelocity;
	static Vector3 vVelocity;
	static SceneEffect* instance;
	SpriteRenderer* fader{ nullptr };

	class GameManager* gameManager{ nullptr };
	class Player* player{ nullptr };


	void Awake() override;

	void Update() override;



	// 화면 암전
	static void FadeOut(float targetAlpha, float duration);
	// 암전됐던 화면 다시 밝게
	static void FadeIn(float duration);
	
	// 카메라 스케일 변경 / 클 수록 오브젝트는 작아짐
	static void Zoom(float targetScale, float duration);
	//static void Zoom(GameObject* t1, GameObject* t2, float duration);

	// 카메라를 부드럽게 흔들기
	static void ShakeCamera(float magnitude, float duration);
	static void ShakeCameraHorizontal(float magnitude, float duration, int frequency = 10);
	static void ShakeCameraVertical(float magnitude, float duration, int frequency = 10);
	// 카메라를 단일 게임 오브젝트가 중앙에 오도록 이동
	static void SmoothMove(float targetX, float duration);
	// 카메라 특정 각도로 부드럽게 회전
	static void RotateCamera(float degree, float duration);

	// 카메라를 원래 상태(포지션 (0,0), 회전 0)로 부드럽게 되돌리기
	static void ResetCamera(float duration);


	// 공격 이펙트는 Player_Attack::Attack,KnockbackAttack에서 직접 관리함
	//static void PlayAttackEffectSequence(GameObject* attacker, GameObject* victim);
	//static void PlayKnockbackEffectSequence(GameObject* attacker, GameObject* victim);

	static void PlayerAttackEnter(float shakeMag, float shakeDuration);
	static void PlayerAttackExit(float shakeMag, float shakeDuration);

	static void PlayerAttackEnter2(float shakeMag, float shakeDuration);
	static void PlayerAttackExit2(float shakeMag, float shakeDuration);
};

