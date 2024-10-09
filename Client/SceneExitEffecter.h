#pragma once
#include "Engine.h"


class SceneExitEffecter : public Object
{
	SpriteRenderer* shader = nullptr;
public:
	void Awake();

	template <typename T>
	void ChangeSceneAfterSceneExitEffect();
};

template<typename T>
inline void SceneExitEffecter::ChangeSceneAfterSceneExitEffect()
{
	shader->enable = true;
	shader->color.A = 0.0f;

	AddTimerFunction([=](bool lu)
		{
			shader->color.A += 1.0f * Time::GetDeltaTime();
			if (lu)
			{
				shader->color.A = 1.0f;
				SceneManager::LoadScene<T>();
			}
		}, true, 1.0f, 1);
}
