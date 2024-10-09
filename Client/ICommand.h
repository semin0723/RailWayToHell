#pragma once
#include <functional>

struct ICommand
{
public:
	virtual ~ICommand() = default;
	/**
	 * @brief 함수객체에 때른 싱행 명령
	 * @return 함수객체의 실행이 끝났는지
	 */
	virtual bool Do() = 0;
	//virtual void UnDo() {}
};

struct MoveCommand : public ICommand
{

	void PostMove();

	class GameManager* gamemanager;
};

struct FunctionCommand : public ICommand
{
	virtual bool Do() { return doFunction(); }

	std::function<bool()> doFunction;
	std::function<bool()> undoFunction;
};