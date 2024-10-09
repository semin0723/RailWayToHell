#include <vector>
#include "SceneManager.h"
#include "Core.h"
#include "Time.h"
#include "Object.h"

void SceneManager::ApplySceneChange()
{
	DG::Core& GM = DG::Core::GetInstance();
	if (GM.GetNextScene() == nullptr) return;


	// ���� �����ϱ� ���� ���� ���� dontDestroyOnSceneChange == true�� ������Ʈ ����
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

	// ���� �ε��Ǵ� ���� ������ �ð��� �ݿ����� �ʵ��� �ʱ�ȭ
	Time::GetInstance().InitTime();
	DG::Core::GetInstance().Reset();
}