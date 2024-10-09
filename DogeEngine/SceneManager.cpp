#include <vector>
#include "SceneManager.h"
#include "Core.h"
#include "Time.h"
#include "Object.h"

void SceneManager::ApplySceneChange()
{
	DG::Core& GM = DG::Core::GetInstance();
	if (GM.GetNextScene() == nullptr) return;


	// 씬을 변경하기 전에 현재 씬의 dontDestroyOnSceneChange == true인 오브젝트 보존
	std::vector<Object*> objectDontDestroyed;
	if (GM.GetCurrentScene() != nullptr)
	{
		auto& objects = GM.GetCurrentScene()->GetObjectList();
		for (auto curObj : objects)
		{
			curObj->BeforeSceneChange();
			if (curObj->GetDontDestroyOnSceneChange() == true)
			{
				if (curObj->GetInitializeOnSceneChange() == true)
				{
					curObj->InitializeObjectState();
				}
				objectDontDestroyed.push_back(curObj);
			}
		}

		delete GM.GetCurrentScene();
	}
	GM.SetCurrentScene(GM.GetNextScene());
	GM.SetNextScene(nullptr);

	for (int i = 0; i < objectDontDestroyed.size(); i++)
	{
		GM.GetCurrentScene()->AddExistingObject(objectDontDestroyed[i]);
		objectDontDestroyed[i]->AfterSceneChange();
	}

	GM.GetCurrentScene()->InitializeScene();

	// 씬이 로딩되는 동안 누적된 시간이 반영되지 않도록 초기화
	Time::GetInstance().InitTime();
	DG::Core::GetInstance().Reset();
}