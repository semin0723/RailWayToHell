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

	State() {} // 생성자는 숨겨놓고 CreateState로 생성하면서 소유객체를 설정하면 될 듯
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