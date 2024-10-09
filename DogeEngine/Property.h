#pragma once

#include <string>
#include <cstdlib>

namespace YAML
{
	class Node;
}
using Archive = YAML::Node;
#include "Archive.h"
#include "Type.h"

class Property abstract
{
public:
	Property(int offset, int size) : offset(offset), size(size) {}
	virtual ~Property() = default;

public:
	virtual void Parse(const Archive& source, void* dest, const void* objectPtr) const = 0;
	virtual void Stringify(Archive& dest, const void* source, const void* objectPtr) const = 0;

	virtual void DeSerialize(const Archive& source, void* dest) const
	{
		Parse(source, (char*)dest + offset, dest);
	}
	virtual void Serialize(Archive& dest, const void* source) const
	{
		Stringify(dest, (char*)source + offset, source);
	}

	virtual void SetValue(const void* source, void* dest) const
	{
		memcpy((char*)dest + offset, source, size);
	}
	virtual void GetValue(void* dest, const void* source) const
	{
		memcpy(dest, (char*)source + offset, size);
	}

	virtual void DeSerializeVector(const Archive& source, void* dest, const Type* elementType, const void* objectPtr) const = 0;
	virtual void SerializeVector(Archive& dest, const void* source, const Type* elementType, const void* objectPtr) const = 0;


protected:
	int offset;
	int size;
};

template<class T>
class TProperty : public Property
{
public:
	TProperty(int offset, int size = sizeof(T)) : Property(offset, size) {}

public:
	virtual void Parse(const Archive& source, void* dest, const void* objectPtr) const override
	{
		__if_exists (YAML::convert<T>::decode) 
		{
			*(T*)dest = source.as<T>();
		}
	}
	virtual void Stringify(Archive& dest, const void* source, const void* objectPtr) const override
	{
		__if_exists (YAML::convert<T>::decode)
		{
			dest = *(T*)source;
		}
	}

	virtual void DeSerializeVector(const Archive& source, void* dest, const Type* elementType, const void* objectPtr) const override
	{
		std::vector<T>* data = (std::vector<T>*)dest;

		data->reserve((data->size() + source.size()));


		for (YAML::const_iterator iter = source.begin(); iter != source.end(); ++iter)
		{
			
			void* writePos = &data->emplace_back();
			
			if (elementType)
			{
				memcpy(writePos, elementType->GetDefaultObject(), size);
			}
			Parse(*iter, writePos, objectPtr);



		}
	}

	virtual void SerializeVector(Archive& dest, const void* source, const Type* elementType, const void* objectPtr) const override
	{
		std::vector<T>* data = (std::vector<T>*)source;
		YAML::Emitter emitter;
		
		emitter << YAML::BeginSeq;

		for (auto iter = data->begin(); iter != data->end(); ++iter)
		{
			YAML::Node temp;
			Stringify(temp, &(*iter), objectPtr);
			emitter << temp;
		}

		emitter << YAML::EndSeq;


		dest = LoadArchive(emitter.c_str());
	}
};

template<>
class TProperty<bool> : public Property
{
public:
	TProperty(int offset, int size = sizeof(bool)) : Property(offset, size) {}

public:
	virtual void Parse(const Archive& source, void* dest, const void* objectPtr) const override
	{
		*(bool*)dest = source.as<bool>();
	}
	virtual void Stringify(Archive& dest, const void* source, const void* objectPtr) const override
	{
		dest = *(bool*)source;
	}

	virtual void DeSerializeVector(const Archive& source, void* dest, const Type* elementType, const void* objectPtr) const override;
	virtual void SerializeVector(Archive& dest, const void* source, const Type* elementType, const void* objectPtr) const override;
};

template<template<class> class Container>
class TContainerProperty : public Property
{
public:
	TContainerProperty(int offset, int size) : Property(offset, size) {}
public:
	virtual void DeSerializeVector(const Archive& source, void* dest, const Type* elementType, const void* objectPtr) const override
	{

	}
	virtual void SerializeVector(Archive& dest, const void* source, const Type* elementType, const void* objectPtr) const override
	{

	}
};

class BoolProperty : public TProperty<bool>
{
public:
	BoolProperty(int offset = 0) : TProperty<bool>(offset) {}

};
class IntProperty : public TProperty<int>
{
public:
	IntProperty(int offset = 0) : TProperty<int>(offset) {}

};

class FloatProperty : public TProperty<float>
{
public:
	FloatProperty(int offset = 0) : TProperty<float>(offset) {}

};

class DoubleProperty : public TProperty<double>
{
public:
	DoubleProperty(int offset = 0) : TProperty<double>(offset) {}

};


class StringProperty : public TProperty<std::string>
{
public:
	StringProperty(int offset = 0) : TProperty<std::string>(offset) {}

};


class ObjectProperty : public TProperty<class ReflectObject>
{
public:
	ObjectProperty(int size, int offset = 0) : TProperty<class ReflectObject>(offset, size) {}

	virtual void Parse(const Archive& source, void* dest, const void* objectPtr) const override;

	virtual void DeSerializeVector(const Archive& source, void* dest, const Type* elementType, const void* objectPtr) const override
	{

	}
	virtual void SerializeVector(Archive& dest, const void* source, const Type* elementType, const void* objectPtr) const override
	{

	}
};

//template<class T>
//class TObjectProperty : public ObjectProperty
//{
//public:
//	TObjectProperty(int size, int offset = 0) : TProperty<class Object>(offset, size) {}
//
//	virtual void Parse(const Archive& source, void* dest, const void* objectPtr) const override;
//
//	virtual void DeSerializeVector(const Archive& source, void* dest, const Type* elementType, const void* objectPtr) const override
//	{
//
//	}
//	virtual void SerializeVector(Archive& dest, const void* source, const Type* elementType, const void* objectPtr) const override
//	{
//
//	}
//};

class VectorProperty : public TContainerProperty<std::vector>
{
public:
	VectorProperty(int size, int offset, Property* typeProperty, const Type* type) : 
		TContainerProperty<std::vector>(offset, size), 
		typeProperty(typeProperty),
		type(type)
	{

	}

	virtual void Parse(const Archive& source, void* dest, const void* objectPtr) const override
	{
		typeProperty->DeSerializeVector(source, dest, type, objectPtr);
	}
	virtual void Stringify(Archive& dest, const void* source, const void* objectPtr) const override
	{
		typeProperty->SerializeVector(dest, source, type, objectPtr);
	}
	Property* typeProperty;
	const Type* type;
};


class ObjectPtrProperty : public TProperty<class ReflectObject*>
{
public:
	ObjectPtrProperty(int size, int offset, const Type* type) : TProperty<class ReflectObject*>(offset, size), type(type){}

	virtual void Parse(const Archive& source, void* dest, const void* objectPtr) const override;

	virtual void Stringify(Archive& dest, const void* source, const void* objectPtr) const override;
	const Type* type;
};



