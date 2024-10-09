#pragma once

class Collider;
class Object;

struct Collision
{
	Collider* collider;
	Object* gameObject;
	// 겹친 정도는 여기다? 아니면 Collider에?
	//Vector2 overlap{ 0, 0 };

	Collision(Collider* _collider, Object* _gameObject) : collider(_collider), gameObject(_gameObject) {}
};
