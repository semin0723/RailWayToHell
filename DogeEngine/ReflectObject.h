#pragma once


#include <string>
#include <typeinfo>
#include <cstdarg>
#include "Type.h"
#include "Property.h"
#include <type_traits>
#include <vector>
#include "assert.h"



namespace std
{
	template <class _Ty, class _Alloc>
	class vector;
	template <class _Ty, class _Alloc>
	class list;
	template <class _Kty, class _Pr, class _Alloc>
	class set;
	template <class _Kty, class _Ty, class _Pr, class _Alloc>
	class map;
}

template<typename T>
struct is_stl_container : std::false_type {};

template<typename... Args>
struct is_stl_container<std::vector<Args...>> : std::true_type {};

template<typename... Args>
struct is_stl_container<std::list<Args...>> : std::true_type {};

template<typename... Args>
struct is_stl_container<std::set<Args...>> : std::true_type {};

template<typename... Args>
struct is_stl_container<std::map<Args...>> : std::true_type {};

template<typename T>
inline constexpr bool is_stl_container_v = is_stl_container<T>::value;


class Type;
namespace YAML
{
	class Node;
}
using Archive = YAML::Node;


/**
 * Object객체에 적용되는 기술
 *	리플렉션
 *	직렬화
 * Object객체의 문제점
 *	미완성(프로퍼티, 멤버함수)
 *  중복상속불가
 * 
 */
class ReflectObject
{
public:
	ReflectObject() = default;
	virtual ~ReflectObject() = default;
public:
	std::string _GetClassName() const;
	Type& GetType() const;

	virtual void Serialize(Archive& archive) const;
	virtual void DeSerialize(const Archive& archive);


	template<class T = ReflectObject>
	T* GetOuter() { return dynamic_cast<T*>(outer); }

	//테스ㅡㅌ 안해봄,.,,
	template<class T = ReflectObject>
	static void LoadFile(const char* fileName, T*& outer);

	template<class T = ReflectObject>
	static void LoadFiles(const char* fileName, std::vector<T*>& outer);
protected:

	ReflectObject* outer;
	friend ReflectObject* NewObject(const Type* type, ReflectObject* outer);
private:

};

template <class T = ReflectObject>
T* NewObject(ReflectObject* outer)
{
	return (T*)NewObject(&Type::GetType<T>(), outer);
}

inline ReflectObject* NewObject(const Type* type, ReflectObject* outer)
{
	ReflectObject* newObject = type->newObject();
	((ReflectObject*)newObject)->outer = outer;
	return newObject;
}

inline std::string _GetClassName(const type_info& type)
{
	std::string className = type.name();
	size_t pos = className.find("class ");
	if (pos != std::string::npos)
	{
		className.erase(pos, 6);
	}
	else
	{
		size_t pos2 = className.find("struct ");
		if (pos2 != std::string::npos)
		{
			className.erase(pos, 6);
		}
	}
	return className;
}

std::string _GetClassName(const type_info& type);

template<class T = ReflectObject>
std::string _GetClassName()
{
	return _GetClassName(typeid(T));
}

template <class T, typename std::enable_if_t<!std::is_abstract_v<T>, int> = 0>
ReflectObject* CreateObject() { return new T(); }
template <class T, typename std::enable_if_t<std::is_abstract_v<T>, int> = 0>
ReflectObject* CreateObject() { return nullptr; }



template<class T>
void ReflectObject::LoadFile(const char* fileName, T*& outer)
{
	static_assert(std::is_base_of_v<ReflectObject, T>);

	Archive archive = LoadArchiveFromFile(fileName);

	T* newObject = (T*)Type::GetType<T>().newObject();
	newObject->DeSerialize(archive);
	*outer = newObject;
}

template<class T>
void ReflectObject::LoadFiles(const char* fileName, std::vector<T*>& outer)
{
	static_assert(std::is_base_of_v<ReflectObject, T>);
	Archive archive = LoadArchiveFromFile(fileName);

	for (auto iter = archive.begin(); iter != archive.end(); iter++)
	{
		T* newObject = (T*)Type::GetType<T>().newObject();
		newObject->DeSerialize(*iter);
		outer.push_back(newObject);
	}
}



#define RegisterMemberClass(value)	\
static inline MemberRegister registerMember##value = MemberRegister::Get<decltype(value)>(::_GetClassName<THISCLASS>(), #value, &THISCLASS::value);

#define ReflectMemeber(memberMacro)			\
memberMacro(RegisterMemberClass)

#define GENERATED_BODY(class, base) 								\
private:															\
	using THISCLASS = class;										\
	using Base = base;												\
	static inline TypeRegister<THISCLASS, base> typeRegister;				\
	ReflectMemeber(class##Reflect)									\
public:																\
	static ReflectObject* CreateObject() { return ::CreateObject<THISCLASS>(); }		\


////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
// 
// VV 리플렉션 관련코드 안봐도됨


class MemberRegister final
{
private:
	MemberRegister(std::string_view className, std::string_view propertyName);
public:
	~MemberRegister();
public:
	template<class Type>
	static MemberRegister Get(std::string_view className, std::string_view propertyName, ...);
	
private:
	template<class PropertyType>
	static void RegisterProperty(std::string_view className, std::string_view propertyName, int offset, int size = sizeof(PropertyType));
	
	template<class T, typename std::enable_if<!std::is_pointer_v<T>, int>::type = 0>
	static Property* GetProperty(int offset, int size)
	{
		assert(0);
		return nullptr;
	}

	template<class T, typename std::enable_if<std::is_pointer_v<T>, int>::type = 0>
	static Property* GetProperty(int offset, int size)
	{
		using RemovePointer = std::remove_pointer_t<T>;
		if constexpr (std::is_base_of_v<ReflectObject, RemovePointer>)
		{
			return new ObjectPtrProperty(size, offset, &Type::GetType<RemovePointer>());
		}
	}

	template<class PropertyType, typename std::enable_if<!is_stl_container_v<PropertyType>, int>::type = 0>
	static Property* RegisterPropertyHelper(std::string_view className, std::string_view propertyName, int offset, int size)
	{
		if constexpr (std::is_base_of_v<ReflectObject, PropertyType >)
		{
			return GetProperty<ReflectObject>(offset, size);
		}
		else
		{
			return GetProperty<PropertyType>(offset, size);
		}
	}

	template<class PropertyType, typename std::enable_if<is_stl_container_v<PropertyType>, int>::type = 0>
	static Property* RegisterPropertyHelper(std::string_view className, std::string_view propertyName, int offset, int size)
	{
		if constexpr (std::is_same_v<PropertyType, std::vector<typename PropertyType::value_type>>)
		{
			if constexpr (std::is_base_of_v<ReflectObject, typename PropertyType::value_type>)
			{
				return new VectorProperty(size, offset, GetProperty<ReflectObject>(0, sizeof(PropertyType::value_type)), &Type::GetType<typename PropertyType::value_type>());
			}
			else
			{
				return new VectorProperty(size, offset, GetProperty<typename PropertyType::value_type>(0, sizeof(PropertyType::value_type)), nullptr);
			}
		}
	}
	std::string className;
	std::string propertyName;
};

template<> static inline Property* MemberRegister::GetProperty<ReflectObject>(int offset, int size)
{
	return new ObjectProperty(size, offset);
}
template<> static inline Property* MemberRegister::GetProperty<bool>(int offset, int size)
{
	return new BoolProperty(offset);
}

template<> static inline Property* MemberRegister::GetProperty<int>(int offset, int size)
{
	return new IntProperty(offset);
}
template<> static inline Property* MemberRegister::GetProperty<float>(int offset, int size)
{
	return new FloatProperty(offset);
}
template<> static inline Property* MemberRegister::GetProperty<double>(int offset, int size)
{
	return new DoubleProperty(offset);
}

template<> static inline Property* MemberRegister::GetProperty<std::string>(int offset, int size)
{
	return new StringProperty(offset);
}

template<class PropertyType>
inline void MemberRegister::RegisterProperty(std::string_view className, std::string_view propertyName, int offset, int size)
{
	Property* registerProperty = nullptr;

	registerProperty = RegisterPropertyHelper<PropertyType>(className, propertyName, offset, size);


	Type::GetType(className.data()).AddProperty(propertyName.data(), registerProperty);
}


template<class Type>
MemberRegister MemberRegister::Get(std::string_view className, std::string_view propertyName, ...)
{
	va_list vl;
	va_start(vl, propertyName);
	int offset = va_arg(vl, int);
	va_end(vl);

	if constexpr (std::is_base_of_v<ReflectObject, Type>)
	{
		RegisterProperty<Type>(className, propertyName, offset);

	}
	else if constexpr (is_stl_container_v<Type>)
	{
		RegisterProperty<Type>(className, propertyName, offset);
	}
	else if constexpr (std::is_class_v<Type> || std::is_same_v<std::string, Type>)
	{
		RegisterProperty<Type>(className, propertyName, offset);
	}
	else if constexpr (std::is_function_v<Type>)
	{

	}
	else
	{
		RegisterProperty<Type>(className, propertyName, offset);
	}
	return MemberRegister(className, propertyName);
}


