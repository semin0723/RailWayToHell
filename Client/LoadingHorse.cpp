#include <thread>
#include "LoadingHorse.h"
#include "TitleScene.h"
#include "SampleScene.h"

extern std::atomic<bool> load_complete;
extern std::atomic<bool> Video_complete;

void LoadingHorse::Awake()
{
	ResourceManager::AddGIFImageByPath("UI/loading", L"Resource/UI/loading.gif");
	std::shared_ptr<Animation> anim = ResourceManager::AddAnimation("UI/loading");
	anim.get()->AddGIF("UI/loading");

	renderer = AddComponent<SpriteRenderer>();
	animator = AddComponent<Animator>();
	animator->SetAnimation(*anim);
	renderer->SetSizeAuto();
}

void LoadingHorse::OnDestroy()
{
	ResourceManager::DeleteGIFImage("UI/loading");
	ResourceManager::DeleteAnimation("UI/loading");
}

void LoadingHorse::Update()
{
	if (load_complete)
	{
		if (Input::IsDown(KeyCode::Esc) || Input::IsDown(KeyCode::Enter) || Video_complete)
		{
			elapsedtime += Time::GetDeltaTime();
			if (elapsedtime > 1.0f)
			{
				SceneManager::LoadScene<SampleScene>();
			}

		}
	}

}