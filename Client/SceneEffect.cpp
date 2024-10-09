#include "SceneEffect.h"
#include "Player.h"
#include "GameManager.h"


SceneEffect* SceneEffect::instance = nullptr;
float SceneEffect::fVelocity{ 0 };
Vector3 SceneEffect::vVelocity{ 0, 0, 0 };

void SceneEffect::Awake()
{
	ChangeName("SceneEffect");

	SetObjectLayer(ObjectLayer::UI);
	fader = AddComponent<SpriteRenderer>();
	fader->SetImage("shade");

	fader->SetSize(Camera::GetMainCamera()->width, Camera::GetMainCamera()->height);
	SetDrawingPriority(9999999);

	fader->enable = false;

	SceneEffect::instance = this;
}

void SceneEffect::Update()
{
	if (mainCam == nullptr)
	{
		mainCam = Camera::GetMainCamera();
	}
	if (CamObj == nullptr)
	{
		CamObj = Camera::GetMainCamera()->GetOwner();
	}

	if (damping)
	{
		mainCam->scale = Math::SmoothDamp(mainCam->scale, targetScale, fVelocity, duration);
		CamObj->transform->SetPosition(Vector3::SmoothDamp(CamObj->transform->GetPosition(), targetPos, vVelocity, duration));
	}

	if (shakeTrain)
	{
		if (gameManager == nullptr) gameManager = GetOwner()->GetObjectByName<GameManager>("GameManager");
		if (player == nullptr) player = GetOwner()->GetObjectByName<Player>("Player");

		if (gameManager->GetCurrentPhase() == Phase::Player_Action)
		{
			if (player->phase == PlayerPhase::Aim) return;
		}

		elapsedTime += Time::GetDeltaTime();

		if (elapsedTime >= shakeTrainInterval)
		{
			elapsedTime -= shakeTrainInterval;
			//printf("%f\n", CamObj->transform->GetPosition().y);
			ShakeCameraVertical(shakeTrainMagnitude, shakeTrainDuration);
		}
	}

	/*if (Input::IsDown(KeyCode::A))
	{
		FadeOut(1.0f, 0.5f);
	}
	else if (Input::IsDown(KeyCode::S))
	{
		FadeIn(1.5f);
	}
	else if (Input::IsDown(KeyCode::D))
	{
		Player* p = GetOwner()->GetObjectByName<Player>("Player");
		Zoom(p, 1.0f);
	}
	else if (Input::IsDown(KeyCode::F))
	{
		ResetCamera(0.7f);
	}
	else if (Input::IsDown(KeyCode::G))
	{
		Player* p = GetOwner()->GetObjectByName<Player>("Player");
		SmoothMove(p, 0.7f);
	}
	else if (Input::IsDown(KeyCode::Z))
	{
		ShakeCamera(200.0f, 1.0f);
	}
	else if (Input::IsDown(KeyCode::X))
	{
		RotateCamera(45, 1.0f);
	}*/
}

void SceneEffect::FadeOut(float targetAlpha, float duration)
{
	instance->fader->enable = true;
	instance->fader->color.A = 0.0f;
	instance->AddRealTimeTimerFunction([targetAlpha, duration](bool lu)
		{
			instance->fader->color.A += targetAlpha * Time::GetRealDeltaTime() / duration;
		}, true, duration, 1);
}

void SceneEffect::FadeIn(float duration)
{
	if (instance->fader->enable == false) return;

	float initAlpha = instance->fader->color.A;
	instance->AddRealTimeTimerFunction([initAlpha, duration](bool lu)
		{
			instance->fader->color.A -= initAlpha * Time::GetRealDeltaTime() / duration;
		}, true, duration, 1);
}

void SceneEffect::Zoom(float targetScale, float duration)
{
	float initScale = instance->mainCam->scale;
	float t = 0.0f;
	instance->AddRealTimeTimerFunction([=](bool lu) mutable
		{
			t += Time::GetRealDeltaTime() / duration;
			instance->mainCam->scale = Math::EaseOutCubic(initScale, targetScale, t);
		}, true, duration, 1);
}

//void SceneEffect::Zoom(GameObject* firstTarget, GameObject* secondTarget, float duration)
//{
//	if (firstTarget == nullptr || secondTarget == nullptr) return;
//
//	float initScale = instance->mainCam->scale;
//	float endScale = 0.8f;
//	float t1 = 0.0f;
//	instance->AddRealTimeTimerFunction([=](bool lu) mutable
//		{
//			t1 += Time::GetRealDeltaTime() / duration;
//			instance->mainCam->scale = Math::EaseOutCubic(initScale, endScale, t1);
//		}, true, duration, 1);
//
//	/*float initX = instance->CamObj->transform->GetPosition().x;
//	float endX = (firstTarget->transform->GetPosition().x + secondTarget->transform->GetPosition().x) / 2.0f;
//	float t2 = 0.0f;
//	instance->AddRealTimeTimerFunction([=](bool lu) mutable
//		{
//			t2 += Time::GetRealDeltaTime() / duration;
//			instance->CamObj->transform->SetPositionX(Math::EaseOutCubic(initX, endX, t2));
//		}, true, duration, 1);*/
//}





void SceneEffect::ShakeCamera(float magnitude, float duration)
{
	//instance->damping = false;
	//instance->mainCam->EasyMove_ShakeScale(magnitude, duration, 10, true, true);
	float mult = 3.f;

	float dur = Random::Range(duration * 0.8f, duration * 1.2f);
	float magX = Random::Range(magnitude * 0.8f, magnitude * 1.2f) * mult;
	int freqX = Random::Range(8, 19);
	float magY = Random::Range(magnitude * 0.8f, magnitude * 1.2f) * mult;
	int freqY = Random::Range(8, 19);

	float initMagX = magX;
	float initMagY = magY;

	float radianX = 0;
	const float radPerSecondX = Math::PI * freqX;
	const float maxRadX = radPerSecondX * duration;
	float radianY = 0;
	const float radPerSecondY = Math::PI * freqY;
	const float maxRadY = radPerSecondY * duration;

	float dirX = 1.f;
	float dirY = 1.f;

	instance->CamObj->AddRealTimeTimerFunction([=](bool lastUpdate) mutable
		{
			magX = initMagX * (maxRadX - radianX) / maxRadX;
			magY = initMagY * (maxRadY - radianY) / maxRadY;

			float dt = Time::GetRealDeltaTime();

			instance->CamObj->transform->Translate(std::cos(radianX) * magnitude * dt * dirX, 0, 0);
			radianX += radPerSecondX * dt;

			instance->CamObj->transform->Translate(0, std::cos(radianY) * magnitude * dt * dirY, 0);
			radianY += radPerSecondY * dt;

			if (lastUpdate)
			{
				float initY = instance->CamObj->transform->GetPosition().y;
				float t = 0.0f;
				instance->AddRealTimeTimerFunction([=](bool lu) mutable
					{
						t += Time::GetRealDeltaTime() / 0.3f;
						instance->CamObj->transform->SetPositionY(Math::EaseOutCubic(initY, 0.0f, t));
					}, true, 0.3f, 1);
			}
		}, true, duration, 1);
}

void SceneEffect::ShakeCameraHorizontal(float magnitude, float duration, int frequency)
{
	//instance->damping = false;
	//instance->mainCam->EasyMove_ShakeScale(magnitude, duration, 10, true, true);
	float mult = 3.f;

	float dur = Random::Range(duration * 0.8f, duration * 1.2f);
	float mag = Random::Range(magnitude * 0.8f, magnitude * 1.2f) * mult;
	int freq = Random::Range((int)(frequency * 0.8f), (int)(frequency * 1.2f));

	float initMag = mag;

	float radian = 0;
	const float radPerSecond = Math::PI * freq;
	const float maxRad = radPerSecond * duration;

	float dir = 1.f;

	instance->CamObj->AddRealTimeTimerFunction([=](bool lastUpdate) mutable
		{
			mag = initMag * (maxRad - radian) / maxRad;

			float dt = Time::GetRealDeltaTime();

			instance->CamObj->transform->Translate(std::cos(radian) * magnitude * dt * dir, 0, 0);
			radian += radPerSecond * dt;

			if (lastUpdate)
			{
				float initX = instance->CamObj->transform->GetPosition().x;
				float t = 0.0f;
				instance->AddRealTimeTimerFunction([=](bool lu) mutable
					{
						t += Time::GetRealDeltaTime() / 0.3f;
						instance->CamObj->transform->SetPositionX(Math::EaseOutCubic(initX, 0.0f, t));
					}, true, 0.3f, 1);
			}
		}, true, duration, 1);
}

void SceneEffect::ShakeCameraVertical(float magnitude, float duration, int frequency)
{
	float mult = 3.f;

	float dur = Random::Range(duration * 0.8f, duration * 1.2f);
	float mag = Random::Range(magnitude * 0.8f, magnitude * 1.2f) * mult;
	int freq = Random::Range((int)(frequency * 0.8f), (int)(frequency * 1.2f));

	float initMag = mag;

	float radian = 0;
	const float radPerSecond = Math::PI * freq;
	const float maxRad = radPerSecond * duration;

	float dir = 1.f;

	instance->CamObj->AddRealTimeTimerFunction([=](bool lastUpdate) mutable
		{
			mag = initMag * (maxRad - radian) / maxRad;

			float dt = Time::GetRealDeltaTime();

			instance->CamObj->transform->Translate(0, std::cos(radian) * magnitude * dt * dir, 0);
			radian += radPerSecond * dt;

			if (lastUpdate)
			{
				float initY = instance->CamObj->transform->GetPosition().y;
				float t = 0.0f;
				instance->AddRealTimeTimerFunction([=](bool lu) mutable
					{
						t += Time::GetRealDeltaTime() / 0.3f;
						instance->CamObj->transform->SetPositionY(Math::EaseOutCubic(initY, 0.0f, t));
					}, true, 0.3f, 1);
			}
		}, true, duration, 1);
}

void SceneEffect::SmoothMove(float targetX, float duration)
{
	float initX = instance->CamObj->transform->GetPosition().x;
	float t = 0.0f;
	instance->AddRealTimeTimerFunction([=](bool lu) mutable
		{
			t += Time::GetRealDeltaTime() / duration;
			instance->CamObj->transform->SetPositionX(Math::EaseInOutCubic(initX, targetX, t));
		}, true, duration, 1);
}

void SceneEffect::RotateCamera(float degree, float duration)
{
	float initRotation = instance->mainCam->rotation;
	float targetRotation = degree;
	float t = 0.0f;
	instance->AddRealTimeTimerFunction([=](bool lu) mutable
		{
			t += Time::GetRealDeltaTime() / duration;
			instance->mainCam->rotation = Math::EaseOutCubic(initRotation, targetRotation, t);
		}, true, duration, 1);
}

void SceneEffect::ResetCamera(float duration)
{
	float initScale = instance->mainCam->scale;
	float endScale = 1.0f;
	float t1 = 0.0f;
	instance->AddRealTimeTimerFunction([=](bool lu) mutable
		{
			t1 += Time::GetRealDeltaTime() / duration;
			instance->mainCam->scale = Math::EaseOutCubic(initScale, endScale, t1);
		}, true, duration, 1);

	float initX = instance->CamObj->transform->GetPosition().x;
	float endX = 0.0f;
	float t2 = 0.0f;
	instance->AddRealTimeTimerFunction([=](bool lu) mutable
		{
			t2 += Time::GetRealDeltaTime() / duration;
			instance->CamObj->transform->SetPositionX(Math::EaseOutCubic(initX, endX, t2));
		}, true, duration, 1);
}



void SceneEffect::PlayerAttackEnter(float shakeMag, float shakeDuration)
{
	SceneEffect::ShakeCameraHorizontal(shakeMag, shakeDuration);
	Time::SetSpeedMult(0.0f);
}
void SceneEffect::PlayerAttackExit(float shakeMag, float shakeDuration)
{
	SceneEffect::ShakeCameraHorizontal(shakeMag, shakeDuration);
	Time::SetSpeedMult(1.0f);
}



void SceneEffect::PlayerAttackEnter2(float shakeMag, float shakeDuration)
{
	SceneEffect::ShakeCameraHorizontal(shakeMag * 1.5f, shakeDuration);
	SceneEffect::RotateCamera(10, shakeDuration * 0.5f);
	SceneEffect::Zoom(0.8f, shakeDuration * 0.5f);
	SceneEffect::SmoothMove(instance->GetOwner()->GetObjectByName("Player")->transform->GetPosition().x, shakeDuration * 0.5f);

	Time::SetSpeedMult(0.0f);
}
void SceneEffect::PlayerAttackExit2(float shakeMag, float shakeDuration)
{
	SceneEffect::ShakeCameraHorizontal(shakeMag * 1.5f, shakeDuration);
	SceneEffect::RotateCamera(0, shakeDuration * 0.5f);
	SceneEffect::Zoom(1.0f, shakeDuration * 0.5f);
	SceneEffect::SmoothMove(0.0f, shakeDuration * 0.5f);
	Time::SetSpeedMult(1.0f);
}


//void SceneEffect::PlayAttackEffectSequence(GameObject* attacker, GameObject* victim)
//{
//	float EnterShakeDuration = GetConfig().Effect_EnteringShakeDurationOnPlayerAttack;
//	float ExitShakeDuration = GetConfig().Effect_ExitShakeDurationOnPlayerAttack;
//	float TimeStopDuration = GetConfig().Effect_HitStopDurationOnPlayerAttack;
//
//	float speedMult = 1.0f;
//	ShakeCameraHorizontal(150.0f, EnterShakeDuration);
//	//Zoom(attacker, victim, zoomDuration);
//	speedMult = 0.0f;
//	Time::SetSpeedMult(speedMult);
//
//	instance->CamObj->AddRealTimeTimerFunction([=](bool lu) mutable
//		{
//			//ResetCamera(zoomDuration);
//			ShakeCameraHorizontal(150.0f, ExitShakeDuration);
//			speedMult = 1.0f;
//			Time::SetSpeedMult(speedMult);
//		}, false, TimeStopDuration, 1);
//}
//
//void SceneEffect::PlayKnockbackEffectSequence(GameObject* attacker, GameObject* victim)
//{
//}
