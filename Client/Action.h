#pragma once
#include "Engine.h"

class Action abstract
{
public:
	virtual void operator()() abstract;

	~Action() {}
};

