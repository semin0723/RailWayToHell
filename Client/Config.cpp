#include "Config.h"
#include <string>
#include "PlayerText.h"
using namespace std::string_literals;



const Config& GetConfig()
{
	static Config config;
	if (!config.isLoaded)
	{
		config.isLoaded = true;

		Archive archive = LoadArchiveFromFile("Resource/Config.yaml"s);

		config.DeSerialize(archive);
	}
	return config;
}
