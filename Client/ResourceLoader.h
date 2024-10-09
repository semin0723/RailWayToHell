#pragma once
#include "Engine.h"

struct ResourceInfo
{
	std::string type;
	std::string key;
	std::string path;
	std::string param;
};

class ResourceLoader
{
public:
	static void LoadResource(std::string path);
	static void Load(ResourceInfo& info);
};

