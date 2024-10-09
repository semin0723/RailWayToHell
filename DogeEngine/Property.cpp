

#include "Property.h"
#include "ReflectObject.h"
#include "Archive.h"
#include "yaml-cpp/node/emit.h"
#include <sstream>

void ObjectProperty::Parse(const Archive& source, void* dest, const void* objectPtr) const
{
	((ReflectObject*)dest)->DeSerialize(source);
}

void ObjectPtrProperty::Parse(const Archive& source, void* dest, const void* objectPtr) const
{
	ReflectObject** realdest = (ReflectObject**)dest;

	auto temp = source.begin()->first.as<std::string>();

	if (*realdest == nullptr)
	{
		*realdest = NewObject(&Type::GetType(temp), (ReflectObject*)objectPtr);
	}

	if ((char*)(*realdest)->GetOuter() == objectPtr)
	{
		(*realdest)->DeSerialize(source[temp]);

	}
	
}

void ObjectPtrProperty::Stringify(Archive& dest, const void* source, const void* objectPtr) const
{
	ReflectObject** realsource = (ReflectObject**)source;

	if (*realsource != nullptr && (*realsource)->GetOuter() == objectPtr)
	{
		Archive temp = dest[(*realsource)->_GetClassName()];

		(*realsource)->Serialize(temp);
	}



}




void TProperty<bool>::DeSerializeVector(const Archive& source, void* dest, const Type* elementType, const void* objectPtr) const
{
	std::vector<bool>* data = (std::vector<bool>*)dest;

	data->reserve((data->size() + source.size()));


	for (YAML::const_iterator iter = source.begin(); iter != source.end(); ++iter)
	{

		void* writePos = (void*)data->emplace_back()._Myptr;

		if (elementType)
		{
			memcpy(writePos, elementType->GetDefaultObject(), size);
		}
		Parse(*iter, writePos, objectPtr);



	}
}


void TProperty<bool>::SerializeVector(Archive& dest, const void* source, const Type* elementType, const void* objectPtr) const
{
	std::vector<bool>* data = (std::vector<bool>*)source;
	YAML::Emitter emitter;

	emitter << YAML::BeginSeq;

	for (auto iter = data->begin(); iter != data->end(); ++iter)
	{
		YAML::Node temp;
		Stringify(temp, iter._Myptr, objectPtr);
		emitter << temp;
	}

	emitter << YAML::EndSeq;


	dest = LoadArchive(emitter.c_str());
};