#pragma once
#include <unordered_map>
#include "Component.h"
#include "State.h"

// 미완성.
// 아직은 Scene 루프에서 다루지 않는다. 필요한 경우 컴포넌트 부착하고 Object.Update() 안에서 수동으로 FSM.Update()를 호출할 것.
// 
// 얘를 써먹으려면 유니티처럼 오브젝트에 하드코딩 못 하고 Script 컴포넌트로만 스크립팅 해야 되는 것?? 그게 아니면 State 안에서는 무조건 오브젝트 타입으로 먼저 캐스팅을 하거나 아님 함수객체를 써야 한다는 소리인데 오버헤드가 너무 크지 않을까
// 아니면 OnStateEnter에서 StateBase를 상속하는 넘 안에 owner를 해당 오브젝트 타입 포인터로 형변환해서 변수로 가지고 있게 해도 되긴 된다. 근데 이런 방식은 너무 복잡하고 헷갈림.


class FiniteStateMachine : public Component
{
	State* currentState;
	std::unordered_map<std::string, State*> states;

public:
	template <typename T>
	State* AddState()
	{
		// STL에 정의된 SFINAE 알고리즘들은 타입으로 존재하는지만 확인하기 때문에 전방선언만으로 이용할 수 있다.
		// Assert(!std::is_base_of<State, T>::value, "not a child of State");
		std::string stateType = typeid(T).name();
		if (states.find(stateType) != states.end()) return states[stateType];

		T* state = new T;
		state->state_type = stateType;
		states[stateType] = state;
		state->owner = GetOwner();
		state->Initialize(); // 여기서 초기화

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