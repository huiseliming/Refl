#pragma once
#include "ContainerTemplate.h"

template<typename T>
class TBuiltinType : public RType
{
public:
    TBuiltinType(const std::string& InName)
        : RType(InName)
    {}
    REFL_TYPE_IMPL_VIRTUAL_FUNCTION(T)
};

class RStringType : public RType
{
public:
    RStringType(const std::string& InName)
        : RType(InName)
    {}
    using String = std::string;
    REFL_TYPE_IMPL_VIRTUAL_FUNCTION(String)
};

REFL_API RType* GetStringType();

template<typename T> RType* GetBuiltinType() { return nullptr; }
template<> REFL_API RType* GetBuiltinType<bool>();
template<> REFL_API RType* GetBuiltinType<int8_t>();
template<> REFL_API RType* GetBuiltinType<int16_t>();
template<> REFL_API RType* GetBuiltinType<int32_t>();
template<> REFL_API RType* GetBuiltinType<int64_t>();
template<> REFL_API RType* GetBuiltinType<uint8_t>();
template<> REFL_API RType* GetBuiltinType<uint16_t>();
template<> REFL_API RType* GetBuiltinType<uint32_t>();
template<> REFL_API RType* GetBuiltinType<uint64_t>();
template<> REFL_API RType* GetBuiltinType<float>();
template<> REFL_API RType* GetBuiltinType<double>();

struct REFL_API RTypePtr
{
    ~RTypePtr()
    {
        if (RequestDelete && ReflType)
        {
            delete ReflType;
            ReflType = nullptr;
        }
    }

    RType* ReflType{ nullptr };
    bool RequestDelete{ false };
};


REFL_API std::unordered_map<std::type_index, RTypePtr>& GetReflTypeTable();

template<typename>
constexpr bool GetReflTypeNotSupported = false;

template<typename T>
RType* GetReflType()
{
    static_assert(!std::is_reference_v<T>);
    std::unordered_map<std::type_index, RTypePtr>& ReflTypeTable = GetReflTypeTable();
    std::type_index TypeIndex = std::type_index(typeid(T));
    auto It = ReflTypeTable.find(TypeIndex);
    if (ReflTypeTable.end() != It) return It->second.ReflType;
    RType* RetReflType = nullptr;
    if constexpr (std::is_class_v<T> && HasStaticClass<T>::value)
    {
        RetReflType = T::StaticClass();
        RetReflType->SetTypeFlag(EReflTypeFlag::RTF_Class);
    }
    else if constexpr (std::is_arithmetic_v<T>)
    {
        RetReflType = GetBuiltinType<T>();
        RetReflType->SetTypeFlag(TStaticBuiltinTypeFlag<T>::Value);
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        RetReflType = GetStringType();
        RetReflType->SetTypeFlag(EReflTypeFlag::RTF_String);
    }
    else if constexpr (std::is_enum_v<T>)
    {
        static_assert(GetReflTypeNotSupported<T> && "UNSUPPORTED TYPE");
    }
    else if constexpr (IsStdVector<T>::value)
    {
        RetReflType = new TStdVectorContainer<IsStdVector<T>::ElementType>();
        RetReflType->SetTypeFlag(EReflTypeFlag::RTF_Vector);
    }
    else if constexpr (IsStdSet<T>::value)
    {
        RetReflType = new TStdSetContainer<IsStdSet<T>::KeyType>();
        RetReflType->SetTypeFlag(EReflTypeFlag::RTF_Set);
    }
    else if constexpr (IsStdMap<T>::value)
    {
        RetReflType = new TStdMapContainer<IsStdMap<T>::KeyType, IsStdMap<T>::ValueType>();
        RetReflType->SetTypeFlag(EReflTypeFlag::RTF_Map);
    }
    else
    {
        static_assert(GetReflTypeNotSupported<T> && "UNSUPPORTED TYPE");
    }
    RetReflType->SetSize(sizeof(T));
    ReflTypeTable[TypeIndex].ReflType = RetReflType;
    if constexpr (IsStdVector<T>::value || IsStdSet<T>::value || IsStdMap<T>::value)
    {
        ReflTypeTable[TypeIndex].RequestDelete = true;
    }
    return RetReflType;
}

