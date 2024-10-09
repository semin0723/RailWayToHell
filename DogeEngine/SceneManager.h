#pragma once
#include "Scene.h"
#include "Core.h"


class SceneManager
{
public:
	template <typename T>
	static void ForceLoadScene(); // 최초로 씬을 로드하는 용도

	template <typename T>
	static void LoadScene(); // nextScene을 이용해 안전하게 씬을 변경하는 용도

	static void ApplySceneChange(); // nextScene을 currentScene으로 갈아끼움
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