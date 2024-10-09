#pragma once
#include "Scene.h"
#include "Core.h"


class SceneManager
{
public:
	template <typename T>
	static void ForceLoadScene(); // ���ʷ� ���� �ε��ϴ� �뵵

	template <typename T>
	static void LoadScene(); // nextScene�� �̿��� �����ϰ� ���� �����ϴ� �뵵

	static void ApplySceneChange(); // nextScene�� currentScene���� ���Ƴ���
};



template<typename T>
inline void SceneManager::ForceLoadScene()
{
	LoadScene<T>();
	ApplySceneChange();
}

template<typename T>
inline void SceneManager::LoadScene()
{
	Scene* scene = new T;
	DG::Core::GetInstance().SetNextScene(scene);
}