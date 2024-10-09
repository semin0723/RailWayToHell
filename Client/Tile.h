#pragma once

#include "Engine.h"
class GameObject;
class Trap;

class TileObject : public Object
{
public:
	virtual void Awake() override;
	virtual void OnDestroy() override;
	virtual void Update() override;
	void SetActiveTileSprite(bool isActive);

public:
	GameObject* object{ nullptr };
	Trap* chandelier{ nullptr };
	Trap* gunPowder{ nullptr };

private:
	Object* tileSpriteObject;
};