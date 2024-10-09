#pragma once

#include <string>
#include <map>
#include <assert.h>

class ReflectObject;
class Property;

class Type
{
	friend class TypeRegisterBase;
	using NewObject = ReflectObject * (*)();
private:
	Type(std::string typeName, std::string baseName, NewObject newObject);
	Type(const Type& other) = default;
	Type& operator=(const Type& other) = default;
	Type(Type&& other) = delete;
	Type& operator=(Type&& other) = delete;

	~Type();
public:
	template<class T = ReflectObject>
	static const Type& GetType()
	{
		return GetType(_GetClassName<T>());
	}
	static Type& GetType(const ReflectObject* instance);
	static Type& GetType(const std::string& typeName);

	ReflectObject* GetDefaultObject() const { return defaultObject; }
	void AddProperty(std::string newPropertyName, Property* newProperty)
	{
		assert(newProperty);
		propertys[newPropertyName] = newProperty;
	}
	void ReoveProperty(std::string removePropertyName)
	{
		propertys.erase(removePropertyName);
	}
	const Property* FindProperty(std::string findPropertyName) const
	{
		auto findIter = propertys.find(findPropertyName);
		if (findIter == propertys.end())
		{
			if (baseType)
			{
				return baseType->FindProperty(findPropertyName);
			}
			else
			{
				return nullptr;
			}
		}
		return propertys.find(findPropertyName)->second;
	}

	template<class FN>
	void LoopProperty(FN fn)
	{
		static_assert(std::is_invocable_v<FN, std::map<std::string, Property*>::iterator>);


		for (auto iter = BeginProperty(); iter != EndProperty(); ++iter)
		{
			fn(iter);
		}

		if (baseType)
		{
			baseType->LoopProperty(fn);
		}
	}

	auto BeginProperty() { return propertys.begin(); };
	auto EndProperty() { return propertys.end(); };

public:
	Type* baseType;
	std::string typeName;
	NewObject newObject;
	ReflectObject* defaultObject;
	std::map<std::string, Property*> propertys;
};

class TypeRegisterBase
{
public:
	TypeRegisterBase(std::string typeName, std::string baseName, Type::NewObject newObject);
	~TypeRegisterBase() = default;

protected:
	Type type;
};


template<class T, class Base>
class TypeRegister : public TypeRegisterBase
{
public:
	TypeRegister() : TypeRegisterBase(_GetClassName<T>(), _GetClassName<Base>(), &T::CreateObject)
	{

	}
};