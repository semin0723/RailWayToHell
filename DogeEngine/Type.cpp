#include "Type.h"
#include "ReflectObject.h"
#include <unordered_map>

static inline std::unordered_map<std::string, Type*>& GetRegistry()
{
	static std::unordered_map<std::string, Type*> registry;
	return registry;
}

Type::Type(std::string typeName, std::string baseName, NewObject newObject) :
	typeName(typeName),
	newObject(newObject),
	baseType(&GetType(baseName))
{
	GetRegistry()[typeName] = this;
	defaultObject = newObject();

}

Type::~Type()
{
	GetRegistry().erase(typeName);
	delete defaultObject;
}

Type& Type::GetType(const ReflectObject* instance)
{
	return GetType(instance->_GetClassName());
}

Type& Type::GetType(const std::string& typeName)
{
	return *GetRegistry()[typeName];
}

TypeRegisterBase::TypeRegisterBase(std::string typeName, std::string baseName, Type::NewObject newObject) :
	type(typeName, baseName, newObject)
{
}

MemberRegister::MemberRegister(std::string_view className, std::string_view propertyName) :
	className(className),
	propertyName(propertyName)
{
}

MemberRegister::~MemberRegister()
{
	Type::GetType(className.c_str()).ReoveProperty(propertyName);
}
