#include "GameEffect.h"
#include "GameObject.h"

void GameEffect::Awake()
{
	SetDrawingPriority(OrderLayer::vfx_default);

	spriteRenderer = AddComponent<SpriteRenderer>();
	animator = AddComponent<Animator>();

	for (int i = 1; i <= 13; i++) {
		crossbowFlash.AddFrame("VFX/Crossbow" + std::to_string(i));
	}
	for (int i = 1; i <= 12; i++) {
		muzzleFlash.AddFrame("VFX/Muzzle" + std::to_string(i));
	}
	for (int i = 1; i <= 29; i++) {
		spawnEffect.AddFrame("Effect/SpawnEffect" + std::to_string(i));
	}
	for (int i = 1; i <= 12; i++) {
		trainSpark.AddFrame("VFX/TrainSpark" + std::to_string(i));
	}
	for (int i = 1; i <= 12; i++) {
		chanDrop.AddFrame("VFX/ChanDrop" + std::to_string(i));
	}
	for (int i = 1; i <= 12; i++) {
		explosion.AddFrame("VFX/Explosion" + std::to_string(i));
	}
	for (int i = 1; i <= 12; i++) {
		acidFire.AddFrame("VFX/AcidFire" + std::to_string(i));
	}
	for (int i = 1; i <= 13; i++) {
		claw.AddFrame("VFX/Claw" + std::to_string(i));
	}
	for (int i = 0; i <= 29; i++) {
		spawnWaitEffect.AddFrame("Effect/SpawnWaitEffect" + std::to_string(i));
	}
	for (int i = 1; i <= 12; i++) {
		bite.AddFrame("VFX/Bite" + std::to_string(i));
	}
}

void GameEffect::Update()
{
	//if (targetObject) {
	//	transform->SetPosition(
	//		targetObject->transform->GetPosition().x,
	//		targetObject->transform->GetPosition().y + targetObject->GetComponent<SpriteRenderer>()->GetHeight() * 0.5f,
	//		0
	//	);
	//}

	if (animator->IsAnimationDone()) {
		Kill();
	}
}

void GameEffect::CreateEffect(std::string key, Vector3 position, GameObject* target)
{
	if (key == "VFX/CrossbowAttack") {
		animator->SetAnimation(crossbowFlash);
		animator->SetLoop(false);
	}
	else if (key == "VFX/NormalAttack") {
		animator->SetAnimation(muzzleFlash);
		animator->SetLoop(false);
	}
	else if (key == "Effect/SpawnEffect") {
		animator->SetAnimation(spawnEffect);
		animator->SetInterval(0.035);
		animator->SetLoop(false);
	}
	else if (key == "Effect/SpawnWaitEffect") {
		animator->SetAnimation(spawnWaitEffect);
	}
	else if (key == "VFX/TrainSpark") {
		animator->SetAnimation(trainSpark);
	}
	else if (key == "VFX/ChanDrop") {
		animator->SetAnimation(chanDrop);
		animator->SetLoop(false);
	}
	else if (key == "VFX/Explosion") {
		animator->SetAnimation(explosion);
		animator->SetLoop(false);
	}
	else if (key == "VFX/AcidFire") {
		animator->SetAnimation(acidFire);
		animator->SetLoop(false);
	}
	else if (key == "VFX/Claw") {
		animator->SetAnimation(claw);
		animator->SetLoop(false);
	}
	else if (key == "VFX/Bite") {
		animator->SetAnimation(bite);
		animator->SetLoop(false);
	}
	else {
		animator->SetAnimation(*ResourceManager::GetAnimation(key));
		animator->SetLoop(false);
	}

	transform->SetPosition(position);
	targetObject = target;
}
