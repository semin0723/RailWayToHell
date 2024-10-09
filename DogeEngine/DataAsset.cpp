#include "DataAsset.h"

void DataAsset::DeSerialize(const Archive& archive)
{
	for (auto iter = archive.begin(); iter != archive.end(); iter++)
	{
		auto temp = iter->first.as<std::string>();

		GetType().FindProperty(iter->first.as<std::string>())->DeSerialize(iter->second, this);

	}

}
