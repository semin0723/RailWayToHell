#pragma once
#include <unordered_map>
#include "Component.h"
#include "State.h"

// �̿ϼ�.
// ������ Scene �������� �ٷ��� �ʴ´�. �ʿ��� ��� ������Ʈ �����ϰ� Object.Update() �ȿ��� �������� FSM.Update()�� ȣ���� ��.
// 
// �긦 ��������� ����Ƽó�� ������Ʈ�� �ϵ��ڵ� �� �ϰ� Script ������Ʈ�θ� ��ũ���� �ؾ� �Ǵ� ��?? �װ� �ƴϸ� State �ȿ����� ������ ������Ʈ Ÿ������ ���� ĳ������ �ϰų� �ƴ� �Լ���ü�� ��� �Ѵٴ� �Ҹ��ε� ������尡 �ʹ� ũ�� ������
// �ƴϸ� OnStateEnter���� StateBase�� ����ϴ� �� �ȿ� owner�� �ش� ������Ʈ Ÿ�� �����ͷ� ����ȯ�ؼ� ������ ������ �ְ� �ص� �Ǳ� �ȴ�. �ٵ� �̷� ����� �ʹ� �����ϰ� �򰥸�.


class FiniteStateMachine : public Component
{
	State* currentState;
	std::unordered_map<std::string, State*> states;

public:
	template <typename T>
	State* AddState()
	{
		// STL�� ���ǵ� SFINAE �˰������ Ÿ������ �����ϴ����� Ȯ���ϱ� ������ ���漱������ �̿��� �� �ִ�.
		// Assert(!std::is_base_of<State, T>::value, "not a child of State");
		std::string stateType = typeid(T).name();
		if (states.find(stateType) != states.end()) return states[stateType];

		T* state = new T;
		state->state_type = stateType;
		states[stateType] = state;
		state->owner = GetOwner();
		state->Initialize(); // ���⼭ �ʱ�ȭ

		return state;
	}
	template <typename T>
	void SetState()
	{
		// Assert(!std::is_base_of<State, T>::value, "not a child of State");
		std::string stateType = typeid(T).name();
		if (states.find(stateType) == states.end()) return;
		if (currentState == states[stateType]) return;

		if (currentState != nullptr) currentState->OnStateExit();
		currentState = states[stateType];
		currentState->OnStateEnter();
	}
	void SetState(State* state);

	State* GetCurrentState();
	std::string GetCurrentStateName();


	void Update();
	void FixedUpdate();
	~FiniteStateMachine();
};