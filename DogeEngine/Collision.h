#pragma once

class Collider;
class Object;

struct Collision
{
	Collider* collider;
	Object* gameObject;
	// ��ģ ������ �����? �ƴϸ� Collider��?
	//Vector2 overlap{ 0, 0 };

	Collision(Collider* _collider, Object* _gameObject) : collider(_collider), gameObject(_gameObject) {}
};
