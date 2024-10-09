#pragma once
#include <string>

class Object;
class FiniteStateMachine;

class State
{
	friend class FiniteStateMachine;
protected:
	Object* owner;
	std::string state_type;

	State() {} // �����ڴ� ���ܳ��� CreateState�� �����ϸ鼭 ������ü�� �����ϸ� �� ��
public:
	std::string GetStateType()
	{
		return state_type;
	}

	virtual void Initialize() {}

	virtual void Update() {}
	virtual void FixedUpdate() {}
	virtual void OnStateEnter() {}
	virtual void OnStateExit() {}
	virtual ~State() {}
};