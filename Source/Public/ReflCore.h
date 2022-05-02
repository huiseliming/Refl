#pragma once
#include "ReflExport.h"
#include "CppStandardLibrary.h"
#include "ReflMacro.h"

class RClass;
class REnum;
class RProperty;

struct REFL_API IStaticVariable
{
	static const std::string EmptyString;
	static const std::string TrueString;
	static const std::string FalseString;

};


template<typename T>
struct TStaticClass {
    static RClass* Initializer()
    {
        return nullptr;
    }
};

template<typename T>
struct TStaticEnum {
    static REnum* Initializer()
    {
        return nullptr;
    }
};

template<typename T>
REnum* StaticEnum()
{
    static_assert(std::is_enum_v<T>);
    return nullptr;
}

template<typename T>
struct IsStdVector : std::false_type { using ElementType = void; };
template<typename T>
struct IsStdVector<std::vector<T>> : std::true_type { using ElementType = T; };
static_assert(!IsStdVector<int>::value && IsStdVector<std::vector<int>>::value);

template<typename T>
struct IsStdSet : std::false_type { using KeyType = void; };
template<typename T>
struct IsStdSet<std::set<T>> : std::true_type { using KeyType = T; };
template<typename T>
struct IsStdSet<std::unordered_set<T>> : std::true_type { using KeyType = T; };
static_assert(!IsStdSet<int>::value && IsStdSet<std::set<int>>::value && IsStdSet<std::unordered_set<int>>::value);

template<typename T>
struct IsStdMap : std::false_type
{
    using KeyType = void;
    using ValueType = void;
};
template<typename TKey, typename TValue>
struct IsStdMap<std::map<TKey, TValue>> : std::true_type
{
    using KeyType = TKey;
    using ValueType = TValue;
};
template<typename TKey, typename TValue>
struct IsStdMap<std::unordered_map<TKey, TValue>> : std::true_type
{
    using KeyType = TKey;
    using ValueType = TValue;
};
static_assert(!IsStdMap<int>::value&& IsStdMap<std::map<int, int>>::value&& IsStdMap<std::unordered_map<int, int>>::value);


template<typename T>
struct HasStaticClass
{
private:
    template<typename U, RClass* (*)()> struct Check {};
    template<class C> static std::true_type Test(Check<C, &C::StaticClass>*);
    template<class C> static std::false_type Test(...);
public:
    static constexpr bool value = std::is_same<std::true_type, decltype(Test<T>(nullptr))>::value;
};

template<typename T>
RProperty* NewProperty(const std::string& InName, uint32_t InOffset);


