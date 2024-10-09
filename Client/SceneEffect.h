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



	// ȭ�� ����
	static void FadeOut(float targetAlpha, float duration);
	// �����ƴ� ȭ�� �ٽ� ���
	static void FadeIn(float duration);
	
	// ī�޶� ������ ���� / Ŭ ���� ������Ʈ�� �۾���
	static void Zoom(float targetScale, float duration);
	//static void Zoom(GameObject* t1, GameObject* t2, float duration);

	// ī�޶� �ε巴�� ����
	static void ShakeCamera(float magnitude, float duration);
	static void ShakeCameraHorizontal(float magnitude, float duration, int frequency = 10);
	static void ShakeCameraVertical(float magnitude, float duration, int frequency = 10);
	// ī�޶� ���� ���� ������Ʈ�� �߾ӿ� ������ �̵�
	static void SmoothMove(float targetX, float duration);
	// ī�޶� Ư�� ������ �ε巴�� ȸ��
	static void RotateCamera(float degree, float duration);

	// ī�޶� ���� ����(������ (0,0), ȸ�� 0)�� �ε巴�� �ǵ�����
	static void ResetCamera(float duration);


	// ���� ����Ʈ�� Player_Attack::Attack,KnockbackAttack���� ���� ������
	//static void PlayAttackEffectSequence(GameObject* attacker, GameObject* victim);
	//static void PlayKnockbackEffectSequence(GameObject* attacker, GameObject* victim);

	static void PlayerAttackEnter(float shakeMag, float shakeDuration);
	static void PlayerAttackExit(float shakeMag, float shakeDuration);

	static void PlayerAttackEnter2(float shakeMag, float shakeDuration);
	static void PlayerAttackExit2(float shakeMag, float shakeDuration);
};

