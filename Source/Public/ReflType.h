#pragma once
#include "ReflRecord.h"

#define REFL_TYPE_OPERATOR_FUNCTION_IMPL(CppType)                                                        \
static_assert(!std::is_reference_v<CppType>);                                                            \
void* New        ()                 { return new CppType(); }                                            \
void  Delete     (void* A)          { delete static_cast<CppType*>(A); }                                 \
void  Constructor(void* A)          { new (A) CppType(); }                                               \
void  Destructor (void* A)          { ((const CppType*)(A))->~CppType(); }                               \
void  CopyAssign (void* A, void* B) { *static_cast<CppType*>(A) = *static_cast<CppType*>(B); }           \
void  MoveAssign (void* A, void* B) { *static_cast<CppType*>(A) = std::move(*static_cast<CppType*>(B)); }\

enum EReflTypeFlag : uint32_t
{
    RTF_NoFlag = 0ULL,
    RTF_Void = 1ULL << 0,
    RTF_Bool = 1ULL << 1,
    RTF_SInt8 = 1ULL << 2,
    RTF_SInt16 = 1ULL << 3,
    RTF_SInt32 = 1ULL << 4,
    RTF_SInt64 = 1ULL << 5,
    RTF_UInt8 = 1ULL << 6,
    RTF_UInt16 = 1ULL << 7,
    RTF_UInt32 = 1ULL << 8,
    RTF_UInt64 = 1ULL << 9,
    RTF_Float = 1ULL << 10,
    RTF_Double = 1ULL << 11,
    RTF_String = 1ULL << 12,
    RTF_Enum = 1ULL << 13,
    RTF_Class = 1ULL << 14,
    //RTF_Object       = 1ULL << 15,
    RTF_Vector = 1ULL << 16,
    RTF_Set = 1ULL << 17,
    RTF_UnorderedSet = 1ULL << 18,
    RTF_Map = 1ULL << 19,
    RTF_UnorderedMap = 1ULL << 20,

    RTF_SIntBits = RTF_SInt8 | RTF_SInt16 | RTF_SInt32 | RTF_SInt64,
    RTF_UIntBits = RTF_UInt8 | RTF_UInt16 | RTF_UInt32 | RTF_UInt64,
    RTF_IntBits = RTF_SIntBits | RTF_UIntBits,
    RTF_FloatBits = RTF_Float | RTF_Double,
};

template<typename T> struct TStaticBuiltinTypeFlag { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_NoFlag; };
template<> struct TStaticBuiltinTypeFlag<bool>     { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_Bool;   };
template<> struct TStaticBuiltinTypeFlag<int8_t>   { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_SInt8;  };
template<> struct TStaticBuiltinTypeFlag<int16_t>  { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_SInt16; };
template<> struct TStaticBuiltinTypeFlag<int32_t>  { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_SInt32; };
template<> struct TStaticBuiltinTypeFlag<int64_t>  { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_SInt64; };
template<> struct TStaticBuiltinTypeFlag<uint8_t>  { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_UInt8;  };
template<> struct TStaticBuiltinTypeFlag<uint16_t> { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_UInt16; };
template<> struct TStaticBuiltinTypeFlag<uint32_t> { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_UInt32; };
template<> struct TStaticBuiltinTypeFlag<uint64_t> { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_UInt64; };
template<> struct TStaticBuiltinTypeFlag<float>    { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_Float;  };
template<> struct TStaticBuiltinTypeFlag<double>   { static constexpr EReflTypeFlag Value = EReflTypeFlag::RTF_Double; };

class RType;

class REFL_API RType : public RRecord
{
public:
    RType(const std::string& InName)
        : RRecord(InName)
    {}

    virtual void* New        (            ) = 0;
    virtual void  Delete     (void*       ) = 0;
    virtual void  Constructor(void*       ) = 0;
    virtual void  Destructor (void*       ) = 0;
    virtual void  CopyAssign (void*, void*) = 0;
    virtual void  MoveAssign (void*, void*) = 0;

    uint32_t GetSize() { return Size; }
    uint32_t GetTypeFlag() { return EReflTypeFlag::RTF_NoFlag; }

protected:
    virtual void Register() override
    {
        RRecord::Register();
    }

    virtual void Deregister() override
    {
        RRecord::Deregister();
    }
private:
    uint32_t Size{ 0 };
    uint32_t TypeFlag{ RTF_NoFlag };

public:
    
private:
    void SetSize(uint32_t InSize) { Size = InSize; }
    void SetTypeFlag(EReflTypeFlag InTypeFlag) { TypeFlag = InTypeFlag; }

    template<typename T> friend  RType* GetReflType();

};

#include "ContainerTemplate.h"

template<typename T>
class TBuiltinType : public RType
{
public:
    TBuiltinType(const std::string& InName)
        : RType(InName)
    {}
    REFL_TYPE_OPERATOR_FUNCTION_IMPL(T)
};

class RStringType : public RType
{
public:
    RStringType(const std::string& InName)
        : RType(InName)
    {}
    using String = std::string;
    REFL_TYPE_OPERATOR_FUNCTION_IMPL(String)
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

