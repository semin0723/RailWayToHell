#pragma once
#include <functional>

struct ICommand
{
public:
	virtual ~ICommand() = default;
	/**
	 * @brief �Լ���ü�� ���� ���� ���
	 * @return �Լ���ü�� ������ ��������
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