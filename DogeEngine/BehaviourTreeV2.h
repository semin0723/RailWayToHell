#pragma once

#include <vector>
#include <string>
#include <functional>
#include <map>
#include "ReflectObject.h"

#define BEGINBT namespace BT {
#define ENDBT }

struct UnionData
{
private:
	const std::type_info* dataKey;
	void* dataptr;
	int storage[4];

public:
	~UnionData()
	{
		printf("%s\n", dataKey->name());
		if (!IsLocal())
		{
			delete dataptr;
		}
	}

	template<class T>
	struct Is_Large : std::conditional < sizeof(storage) < sizeof(T), std::true_type, std::false_type > ::type{};

	template<class T>
	T GetValue()
	{
		if (*dataKey == typeid(T))
		{
			return *reinterpret_cast<T*>(dataptr);
		}
		else
		{
			return T();
		}
	}

	template<class T, typename std::enable_if<Is_Large<T>::value, int>::type = 0>
	void SetValue(T&& value)
	{
		using ValueType = std::remove_const_t<std::remove_reference_t<T>>;
		dataKey = &typeid(ValueType);
		dataptr = new ValueType(std::forward<T>(value));

	}
	template<class T, typename std::enable_if<!Is_Large<T>::value, int>::type = 0>
	void SetValue(T&& value)
	{
		using ValueType = std::remove_const_t<std::remove_reference_t<T>>;
		dataKey = &typeid(ValueType);
		dataptr = new (storage) ValueType(std::forward<T>(value));
	}

	template<class T>
	static UnionData CreateValue(T&& value)
	{
		UnionData result;
		result.SetValue(std::forward<T>(value));
		return result;
	}

	template<class T>
	UnionData& operator=(T value)
	{
		SetValue(std::forward<T>(value));
		return *this;
	}

	template<class T>
	operator T()
	{
		return GetValue<T>();
	}
	bool IsLocal()
	{
		return dataptr == storage;
	}

};
static_assert(sizeof(UnionData) == 32);

BEGINBT

namespace Status
{
	enum Type
	{
		Success,
		Failure,
		Running
	};
};

class Node;
class BlackBoard;
class BehaviourTreeV2;
class Decorator;



class BehaviourTreeV2 : public ReflectObject
{
public:
	BehaviourTreeV2() : rootNode(nullptr), state(Status::Success), owner(nullptr) {}
	~BehaviourTreeV2();

public:
	void Behave(float deltaTime);
	void SetOwner(void* owner) { this->owner = owner; }
	template<class T>
	T* GetOwner() { return (T*)owner; }


private:
	Node* rootNode;
	Status::Type state;
	void* owner;


#define BehaviourTreeV2Reflect(x)	\
	x(rootNode)
	GENERATED_BODY(BehaviourTreeV2, ReflectObject)
};


class BlackBoard : public ReflectObject
{
public:
	UnionData& operator[](const std::string& str)
	{
		auto iter = data.find(str);
		if (iter == data.end())
		{
			if (parent)
			{
				return (*parent)[str];
			}
			else
			{
				assert(0);
				return data[str];
			}
		}
		return iter->second;
	}

	void SetParent(BlackBoard* newParent) { parent = newParent; }
private:
	BlackBoard* parent;
	std::map<std::string, UnionData> data;
};

class Node : public ReflectObject
{
public:
	Node() : blackBoard(nullptr) {}
	virtual ~Node();

	virtual void OnEnter() {}
	virtual void OnExit() { blackBoard = nullptr; }
	virtual Status::Type OnBehave(float deltaTime) = 0;

	Status::Type Behave(float deltaTime);

	Node* GetParentNode() { return GetOuter<Node>(); }
	BehaviourTreeV2* GetBehaviourTree();

	template<class T>
	T* GetBehaviourTree()
	{
		return (T*)GetBehaviourTree();
	}
	template<class T>
	T* GetOwner()
	{
		return GetBehaviourTree()->GetOwner<T>();
	}
	
	BlackBoard* blackBoard;


#define NodeReflect(x)	\

	GENERATED_BODY(Node, ReflectObject)
};

class ActionNode : public Node
{
public:


#define ActionNodeReflect(x)
	GENERATED_BODY(ActionNode, Node)
};

class ControlNode : public Node
{
public:
	ControlNode() : currentNode() {}
	virtual ~ControlNode();
public:
	virtual void OnEnter() override;
	virtual void OnExit()  override;

	Node* GetCurrentNode() { return *currentNode; }
	void NextNode();

	std::vector<Node*> childNodes;
	std::vector<Node*>::iterator currentNode;

#define ControlNodeReflect(x)	\
	x(childNodes) \

	GENERATED_BODY(ControlNode, ReflectObject)
};

class SequenceNode : public ControlNode
{
public:
	virtual Status::Type OnBehave(float deltaTime) override;


#define SequenceNodeReflect(x)

	GENERATED_BODY(SequenceNode, ControlNode)
};

class SelectorNode : public ControlNode
{
public:
	virtual Status::Type OnBehave(float deltaTime) override;
#define SelectorNodeReflect(x)

	GENERATED_BODY(SelectorNode, ControlNode)
};

class Decorator : public Node
{
public:
	Decorator() = default;
	virtual ~Decorator();

public:
	virtual void OnEnter() override { nextNode->OnEnter(); }
	virtual void OnExit()  override { nextNode->OnEnter(); }
	virtual Status::Type OnBehave(float deltaTime) override { return nextNode->Behave(deltaTime); }

protected:
	Node* nextNode;


#define DecoratorReflect(x) \
	x(nextNode)


	GENERATED_BODY(Decorator, Node)
};

class BlackBoardDecorator : public Decorator
{
public:
	virtual Status::Type OnBehave(float deltaTime) override;


public:
	BlackBoard blackboardOrigin;



#define BlackBoardDecoratorReflect(x)
	GENERATED_BODY(BlackBoardDecorator, Decorator)
};


class ForceSuccess : public BT::Decorator
{
	virtual Status::Type OnBehave(float deltaTime) override
	{
		if (Status::Type result = Decorator::OnBehave(deltaTime); result == Status::Running)
		{
			return Status::Running;
		}
		return Status::Success;
	}
#define ForceSuccessReflect(x)
	GENERATED_BODY(ForceSuccess, Decorator)
};

ENDBT
