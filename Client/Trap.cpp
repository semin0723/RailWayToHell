#include "Trap.h"
#include "GameManager.h"
#include "ICommand.h"
#include "Selectable.h"
#include "Highlightable.h"
#include "PlayerText.h"
#include "Player.h"
#include "ChatBalloonComponent.h"
#include <random>
#include <format>
#include "GameEffect.h"


void TrapHit::operator()(int damage)
{
	owner->OnClicked();


	AudioFramework::Play_BankSound("Sound", owner->hitSoundName);
}

void Trap::Awake()
{
	trapdie.owner = this;

	type = EType::Structure;
	AddComponent<HealthComponent>()->Init(1, &trapdie, nullptr);
}

void Trap::Start()
{
	eventManager = AddComponent<EventManager>();
	AddComponent<CircleCollider>();
	AddComponent<Selectable>();


	gameManager = GetOwner()->GetObjectByName<GameManager>("GameManager");
}

struct TrapCommand : public ICommand
{
	virtual bool Do() override
	{
		return trap->InvokeAttack();
	}

	Trap* trap;
};



void Trap::OnClicked()
{
	TrapCommand* trapCommand = new TrapCommand();
	trapCommand->trap = this;
	gameManager->RegisterCommandList(ActSpeed::InteractableObject, trapCommand);
}


void Chandelier::Awake()
{
	Trap::Awake();
	attackDamage = 3;
	RelativePosition = Vector3(0.0f, 415.0f, 0.0f);
	SpriteRenderer* sprite = AddComponent<SpriteRenderer>();
	sprite->SetImage("Trap/Chandelier");
	sprite->SetSizeAuto();
	sprite->MultSize(1.2f);
	GetComponent<HealthComponent>()->SetUIYPos(sprite->GetHeight());
	SetDrawingPriority(OrderLayer::Chandelier);

	AddComponent<Highlightable_Chandelier>();


	hitSoundName = "SFX_Chandelier_Hit";
	attackSoundName = "SFX_Chandelier";
}

bool Chandelier::InvokeAttack()
{
	if (isFall == false)
	{
		float initY = transform->GetPosition().y;
		float goalY = -352.0f + GetComponent<SpriteRenderer>()->GetHeight() * 0.45f;

		float t = 0.0f;
		//printf("move time: %f\n", moveTime);
		AddTimerFunction([this, initY, goalY, t](bool lu) mutable
			{
				//printf("test... %f\n", t);
				t += Time::GetDeltaTime() / moveTime;
				transform->SetPositionY(Math::EaseInCubic(initY, goalY, t));
			}, true, moveTime, 1);

		isFall = true;
	}

	elapsedtime += Time::GetDeltaTime();
	if (elapsedtime > moveTime + 0.1F)
	{
		isAttackSucces = gameManager->GetTile(GetCurrentTileIndex())->object;
		gameManager->DamageInTile(GetCurrentTileIndex(), attackDamage);
		if (isAttackSucces && !gameManager->GetTile(GetCurrentTileIndex())->object)
		{

			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_int_distribution<int> dis(13, 16);

			int randomNum = dis(gen);

			const PlayerText& text = PlayerTextManager().GetText(randomNum);

			if (randomNum == 14) {
				gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->
					BubbleDialog(std::vformat(std::string_view(text.BubbleText), std::make_format_args("샹들리에")), text.FontSize, text.BubbleDuration, false, Vector3(-10.f, 0, 0));
			}
			else if (randomNum == 13) {
				gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->
					BubbleDialog(std::vformat(std::string_view(text.BubbleText), std::make_format_args("샹들리에")), text.FontSize, text.BubbleDuration, false, Vector3(-10.f, 0, 0));
			}
			else if (randomNum == 16) {
				gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->
					BubbleDialog(std::vformat(std::string_view(text.BubbleText), std::make_format_args("샹들리에")), text.FontSize, text.BubbleDuration, false, Vector3(20.f, 0, 0));
			}
			else {
				gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->
					BubbleDialog(std::vformat(std::string_view(text.BubbleText), std::make_format_args("샹들리에")), text.FontSize, text.BubbleDuration);
			}
			
		}

		// TODO: 여기에 샹들리에 VFX 추가.
		GameEffect* effect = AddObject<GameEffect>();
		effect->CreateEffect("VFX/ChanDrop", transform->GetPosition() + Vector3(0, 40.f, 0));
		effect->GetComponent<SpriteRenderer>()->SetSizeAuto();

		gameManager->DestroyObjectInTile(this);

		AudioFramework::Play_BankSound("Sound", attackSoundName);
		return true;
	}
	return false;
}

void GunPowder::Awake()
{
	Trap::Awake();
	attackDamage = 1;
	RelativePosition = Vector3(100.0f, 75.0f, 0.0f);
	SpriteRenderer* sprite = AddComponent<SpriteRenderer>();
	sprite->SetImage("Trap/GunPowder");
	sprite->SetSizeAuto();
	GetComponent<HealthComponent>()->SetUIYPos(sprite->GetHeight());
	SetDrawingPriority(OrderLayer::GunPowder);

	AddComponent<Highlightable_Gunpowder>();

	hitSoundName = "SFX_Gunpowder_Hit";
	attackSoundName = "SFX_Gunpowder";
}

bool GunPowder::InvokeAttack()
{
	if (isInit == false)
	{
		isInit = true;

		CreateEffect();

		int idx = GetCurrentTileIndex();
		left_victim = gameManager->GetTile(idx)->object;
		if (left_victim != nullptr) Hit(idx);
		left_victim = gameManager->GetTile(idx)->object;

		if (idx + 1 < gameManager->InitTileSize)
		{
			right_victim = gameManager->GetTile(idx + 1)->object;
			if (right_victim != nullptr) Hit(idx + 1);
			right_victim = gameManager->GetTile(idx + 1)->object;
		}
	}

	elapsedTime += Time::GetDeltaTime();
	if (elapsedTime < time_gap_between_hit_and_knockback)
	{
		return false;
	}

	int randomNum = Random::Range(1, 4);

	PlayerText& text = PlayerTextManager().GetText(randomNum);
	if (randomNum == 3) {
		gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->
			BubbleDialog(std::vformat(std::string_view(text.BubbleText), std::make_format_args("화약통")), text.FontSize, text.BubbleDuration, false, Vector3(15.f, 0, 0));
	}
	else if (randomNum == 1) {
		gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->
			BubbleDialog(std::vformat(std::string_view(text.BubbleText), std::make_format_args("화약통")), text.FontSize, text.BubbleDuration, false, Vector3(-5.f, 0, 0));
	}
	else if (randomNum == 4) {
		gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->
			BubbleDialog(std::vformat(std::string_view(text.BubbleText), std::make_format_args("화약통")), text.FontSize, text.BubbleDuration, false, Vector3(30.f, 0, 0));
	}
	else {
		gameManager->GetPlayer()->GetComponent<ChatBalloonComponent>()->
			BubbleDialog(std::vformat(std::string_view(text.BubbleText), std::make_format_args("화약통")), text.FontSize, text.BubbleDuration);
	}
	
	if (left_victim != nullptr) Knockback(left_victim, -1);
	if (right_victim != nullptr) Knockback(right_victim, 1);
	
	gameManager->DestroyObjectInTile(this);
	return true;
}

void GunPowder::CreateEffect()
{
	GetComponent<SpriteRenderer>()->color.A = 0.0f;

	// TODO: 여기에 화약통 VFX추가.
	GameEffect* effect = AddObject<GameEffect>();
	effect->CreateEffect("VFX/Explosion", transform->GetPosition());
	effect->GetComponent<SpriteRenderer>()->SetSizeAuto();

	AudioFramework::Play_BankSound("Sound", attackSoundName);
}

void GunPowder::Hit(int index)
{
	gameManager->DamageInTile(index, attackDamage);
}

void GunPowder::Knockback(GameObject* victim, int dir)
{
	gameManager->Knockback(false, victim, dir);
}
