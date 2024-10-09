#include "Boss.h"
#include "BossLeftHand.h"
#include "BossRightHand.h"
#include "MonsterComponent.h"
#include "BossAIComponent.h"
#include "Selectable.h"
#include "Highlightable.h"

void Boss::Awake()
{
	printf("Boss Created\n");
	SetDrawingPriority(OrderLayer::Enemy_Boss_Body);

	curSpawnTileIndex = { -1, -1 };

	for (int i = 1; i <= 120; i++) {
		anim_bossSpawn.AddFrame("Boss/SpawnAnimation" + std::to_string(i));
	}
	for (int i = 1; i <= 30; i++) {
		anim_bossSpawn.AddFrame("Boss/OpenEye" + std::to_string(i));
		anim_bossEyeOpen.AddFrame("Boss/OpenEye" + std::to_string(i));
	}
	for (int i = 30; i >= 1; i--) {
		anim_bossEyeClose.AddFrame("Boss/OpenEye" + std::to_string(i));
	}
	for (int i = 30; i >= 16; i--) {
		anim_bossEyeCloseHalf1.AddFrame("Boss/OpenEye" + std::to_string(i));
	}
	for (int i = 15; i >= 1; i--) {
		anim_bossEyeCloseHalf2.AddFrame("Boss/OpenEye" + std::to_string(i));
	}

	spawnLocation = {
		{1, 4},
		{2, 5},
		{1, 5}
	};

	type = EType::Enemy;
	spriteRenderer = AddComponent<SpriteRenderer>();
	//spriteRenderer->SetImage(ResourceManager::GetImage("Boss/Body"));

	lock = AddObject<Object>();
	lock->AddComponent<SpriteRenderer>();
	lock->GetComponent<SpriteRenderer>()->SetImage(ResourceManager::GetImage("UI/BossLock"));
	lock->GetComponent<SpriteRenderer>()->SetSizeAuto();
	lock->SetDrawingPriority(OrderLayer::hp_bar + 1);
	lock->transform->SetParent(transform);
	lock->transform->SetPositionY(GetComponent<SpriteRenderer>()->GetHeight() + 450.f);
	lock->GetComponent<SpriteRenderer>()->enable = true;

	anim_bossSpawn.SetInterval(0.025f);

	animator = AddComponent<Animator>();
	animator->SetAnimation(anim_bossSpawn, true);
	animator->SetLoop(false);
	spriteRenderer->SetSizeAuto();

	animationState = BossAnimationState::Spawn;

	transform->SetPositionY(-220.f);
	//transform->SetPositionY(-400.f);

	GameObject* bossBackGround = AddObject<GameObject>();
	SpriteRenderer* bossBg = bossBackGround->AddComponent<SpriteRenderer>();
	bossBg->SetImage(ResourceManager::GetImage("Boss/BackBody"));
	bossBg->SetSizeAuto();
	bossBackGround->transform->SetParent(transform);
	bossBackGround->transform->SetPosition(30.f, 220.f, 0);

	bossBackGround->SetDrawingPriority(OrderLayer::Enemy_Boss_BackGround);

	MonsterComponent* monsterComponent = AddComponent<MonsterComponent>();
	monsterComponent->SetEnemyInfo(3, 0, 0, 0, EnemyType::Boss, 0);



	AddComponent<BossAIComponent>();

	BoxCollider* collider = AddComponent<BoxCollider>();
	collider->SetSize(spriteRenderer->GetWidth() * 0.75f, spriteRenderer->GetHeight());
	collider->SetAnchor(0.0f, spriteRenderer->GetHeight() * 0.5f);

	AddComponent<Selectable>();
	AddComponent<Highlightable_Boss>();

	GetComponent<Selectable>()->SetComponentEnable(false);

	GetComponent<MonsterComponent>()->GetHPBarObject()->transform->SetPositionY(150.f);
	GetComponent<MonsterComponent>()->hitSoundName = "SFX_Boss_Hurt";

	SetCurrentTileIndex(3);
	//CreateHand();


	/*AddRealTimeTimerFunction([=](bool lu)
		{
			gameManager->isBossStageReady = true;
		}, false, 1.0f, 1);*/
}

void Boss::Update()
{
	if (waitTurn == 1) {
		if (isHalfClose) return;
		animator->SetAnimation(anim_bossEyeCloseHalf1);
		animator->SetLoop(false);
		spriteRenderer->SetSizeAuto();
		isHalfClose = true;
	}
	else if (waitTurn == 2) {
		animationState = BossAnimationState::EyeClose;
		animator->SetAnimation(anim_bossEyeCloseHalf2);
		animator->SetLoop(false);
		spriteRenderer->SetSizeAuto();
		waitTurn = 0;
	}

	if (animator->IsAnimationDone()) {
		switch (animationState) {
		case BossAnimationState::Spawn:
			animationState = BossAnimationState::Default;
			break;
		case BossAnimationState::EyeClose:
			isOpenEye = false;
			lock->GetComponent<SpriteRenderer>()->enable = true;
			waitTurn = 0;
			printf("CreateHand\n");
			CreateHand();
			animationState = BossAnimationState::Idle;
			break;
		case BossAnimationState::EyeOpen:
			EnableSelectable();
			animationState = BossAnimationState::Idle;
			isHalfClose = false;
			break;
		case BossAnimationState::Idle:
			break;
		default:
			animationChangeInterval -= Time::GetDeltaTime();
			if (animationChangeInterval <= 0) {
				animationChangeInterval = 0.5f;
				animationState = BossAnimationState::EyeClose;
				animator->SetAnimation(anim_bossEyeClose);
				animator->SetLoop(false);
				spriteRenderer->SetSizeAuto();
			}
			break;
		}
	}
}

void Boss::OnDestroy()
{

	if (spawnEffectLeft != nullptr) spawnEffectLeft->Kill();
	if (spawnEffectRight != nullptr) spawnEffectRight->Kill();}

void Boss::EnableSelectable()
{
	GetComponent<Selectable>()->SetComponentEnable(true);
	lock->GetComponent<SpriteRenderer>()->enable = false;
}

void Boss::OpenEye()
{
	if (isOpenEye == true) {
		return;
	}

	animationState = BossAnimationState::EyeOpen;
	animator->SetAnimation(anim_bossEyeOpen);
	animator->SetLoop(false);
	spriteRenderer->SetSizeAuto();

	isOpenEye = true;
	if (GetComponent<MonsterComponent>()->GetHp() == 1) {
		return;
	}

	spawnEffectLeft = AddObject<GameEffect>();
	spawnEffectLeft->CreateEffect(
		"Effect/SpawnWaitEffect", 
		Vector3(
			DEFINE::GetTilePositionX(spawnLocation[(3 - GetComponent<MonsterComponent>()->GetHp())].first),
			-260.f,
			0
		)
	);
	//spawnEffectLeft->GetComponent<SpriteRenderer>()->SetSizeAuto();
	spawnEffectLeft->GetComponent<SpriteRenderer>()->anchorType = AnchorType::Center;
	//sprite->SetSizeAuto();
	spawnEffectLeft->GetComponent<SpriteRenderer>()->SetSize(384.f, 216.f);

	spawnEffectLeft->GetComponent<SpriteRenderer>()->color.G = 0.5f;
	spawnEffectLeft->GetComponent<SpriteRenderer>()->color.B = 0.5f;

	spawnEffectRight = AddObject<GameEffect>();
	spawnEffectRight->CreateEffect(
		"Effect/SpawnWaitEffect",
		Vector3(
			DEFINE::GetTilePositionX(spawnLocation[(3 - GetComponent<MonsterComponent>()->GetHp())].second),
			-260.f,
			0
		)
	);
	//spawnEffectRight->GetComponent<SpriteRenderer>()->SetSizeAuto();
	spawnEffectRight->GetComponent<SpriteRenderer>()->anchorType = AnchorType::Center;
	spawnEffectRight->GetComponent<SpriteRenderer>()->SetSize(384.f, 216.f);

	spawnEffectRight->GetComponent<SpriteRenderer>()->color.G = 0.5f;
	spawnEffectRight->GetComponent<SpriteRenderer>()->color.B = 0.5f;

	curSpawnTileIndex = spawnLocation[(3 - GetComponent<MonsterComponent>()->GetHp())];

}

void Boss::CloseEye()
{
	if (isOpenEye == false) {
		return;
	}
	animationState = BossAnimationState::EyeClose;
	if (isHalfClose) {
		animator->SetAnimation(anim_bossEyeCloseHalf2);
		animator->SetLoop(false);
	}
	else {
		animator->SetAnimation(anim_bossEyeClose);
		animator->SetLoop(false);
	}
	spriteRenderer->SetSizeAuto();
	isOpenEye = false;
	waitTurn = 0;

	isBossStageReady = false;
	gameManager->isBossStageReady = false;
}

void Boss::CreateHand()
{
	if (spawnEffectLeft != nullptr) {
		spawnEffectLeft->Kill();
		spawnEffectLeft = nullptr;
	}
	if (spawnEffectRight != nullptr) {
		spawnEffectRight->Kill();
		spawnEffectRight = nullptr;
	}
	if (curSpawnTileIndex == std::pair<int, int>(-1, -1)) {
		gameManager->SpawnMonsterInTile(spawnLocation[(3 - GetComponent<MonsterComponent>()->GetHp())].first, 3, 0);
		gameManager->SpawnMonsterInTile(spawnLocation[(3 - GetComponent<MonsterComponent>()->GetHp())].second, 3, 0);
	}
	else {
		gameManager->SpawnMonsterInTile(curSpawnTileIndex.first, 3, 0);
		gameManager->SpawnMonsterInTile(curSpawnTileIndex.second, 3, 0);
	}


	// 보스 손 생성
	{
		int leftIdx = -1;
		int rightIdx = -1;

		for (int i = 0; i <= 2; ++i)
		{
			if (gameManager->GetTile(i)->object == nullptr)
			{
				leftIdx = i;
				break;
			}
		}
		if (leftIdx == -1)
		{
			for (int i = 0; i <= 2; ++i)
			{
				if (gameManager->GetTile(i)->object->type == EType::Enemy)
				{
					leftIdx = i;
					gameManager->DamageInTile(leftIdx, 100);
					break;
				}
			}
		}

		for (int i = 6; i >= 4; --i)
		{
			if (gameManager->GetTile(i)->object == nullptr)
			{
				rightIdx = i;
				break;
			}
		}
		if (rightIdx == -1)
		{
			for (int i = 6; i >= 4; --i)
			{
				if (gameManager->GetTile(i)->object->type == EType::Enemy)
				{
					rightIdx = i;
					gameManager->DamageInTile(rightIdx, 100);
					break;
				}
			}
		}
		leftHand = GetOwner()->GetObjectByName<GameManager>("GameManager")->AddGameObjectInTile<BossLeftHand>(leftIdx);
		static_cast<BossLeftHand*>(leftHand)->body = this;

		rightHand = GetOwner()->GetObjectByName<GameManager>("GameManager")->AddGameObjectInTile<BossRightHand>(rightIdx);
		static_cast<BossRightHand*>(rightHand)->body = this;
	}
	




	// 1,4    2,5      2 4  or  1 5
	GetComponent<Selectable>()->SetComponentEnable(false);


	if (isBossStageReady == false)
	{
		gameManager->isBossStageReady = true;
		isBossStageReady = true;
	}
}
GameObject* Boss::GetHand(int index)
{
	if (index == 1) {
		return leftHand;
	}
	else {
		return rightHand;
	}
}

bool Boss::IsHandAlive()
{
	if (rightHand != nullptr || leftHand != nullptr) return true;
	else return false;
}

template <>
void Boss::HandCrashed<BossLeftHand>() {
	leftHand = nullptr;
}

template <>
void Boss::HandCrashed<BossRightHand>() {
	rightHand = nullptr;
}