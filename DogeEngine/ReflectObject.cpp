#include "ReflectObject.h"
#include "Property.h"
#include "Type.h"
#include "Archive.h"
#include <stdarg.h>

#include "yaml-cpp/node/iterator.h"
#include "yaml-cpp/node/detail/node.h"
#include "yaml-cpp/yaml.h"

std::string ReflectObject::_GetClassName() const
{
	return ::_GetClassName(typeid(*this));
}

Type& ReflectObject::GetType() const
{
	return Type::GetType(this);
}

void ReflectObject::Serialize(Archive& archive) const
{
	Type& type = GetType();
	type.LoopProperty([this, &archive](auto iter)
					  {
						  Archive temp = archive[iter->first];
						  iter->second->Serialize(temp, this);
					  });
	//for (auto iter = type.BeginProperty(); iter != type.EndProperty(); ++iter)
	//{
	//	Archive temp = archive[iter->first];
	//	iter->second->Serialize(temp, this);
	//}
}

void ReflectObject::DeSerialize(const Archive& archive)
{
	Type& type = GetType();

	for (YAML::const_iterator iter = archive.begin(); iter != archive.end(); ++iter)
	{
		const Property* poperty = type.FindProperty(iter->first.as<std::string>());
		if (poperty)
		{
			poperty->DeSerialize(iter->second, this);
		}
		else
		{
		}
	}
}
