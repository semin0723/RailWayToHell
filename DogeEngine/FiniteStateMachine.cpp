#include <typeinfo>
#include "FiniteStateMachine.h"

void FiniteStateMachine::SetState(State* state)
{
	std::string stateType = state->GetStateType();
	if (states.find(stateType) == states.end()) return;
	if (currentState == states[stateType]) return;

	if (currentState != nullptr) currentState->OnStateExit();
	currentState = states[stateType];
	currentState->OnStateEnter();
}

State* FiniteStateMachine::GetCurrentState()
{
	return currentState;
}

std::string FiniteStateMachine::GetCurrentStateName()
{
	return typeid(*currentState).name();
}

void FiniteStateMachine::Update()
{
	if (currentState == nullptr) return;
	currentState->Update();
}

void FiniteStateMachine::FixedUpdate()
{
	if (currentState == nullptr) return;
	currentState->FixedUpdate();
}

FiniteStateMachine::~FiniteStateMachine()
{
	for (auto state : states)
	{
		delete state.second;
	}
}
