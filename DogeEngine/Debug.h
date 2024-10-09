#pragma once
#include <iostream>

class Debug
{
public:
	static void Log(const char* text)
	{
#ifdef _DEBUG
		printf(text);
		printf("\n");
#endif
	}
};