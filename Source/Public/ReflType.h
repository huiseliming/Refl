#pragma once
#include "ReflRecord.h"

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

typedef void* (*FPReflNew)        ();
typedef void  (*FPReflDelete)     (void*);
typedef void  (*FPReflConstructor)(void*);
typedef void  (*FPReflDestructor) (void*);
typedef void  (*FPReflAssign)     (void*, void*);

class RType;

template<typename T>
RType* GetReflType();

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

#define REFL_TYPE_IMPL_VIRTUAL_FUNCTION(CppType)                                                         \
static_assert(!std::is_reference_v<CppType>);                                                            \
void* New        ()                 { return new CppType(); }                                            \
void  Delete     (void* A)          { delete static_cast<CppType*>(A); }                                 \
void  Constructor(void* A)          { new (A) CppType(); }                                               \
void  Destructor (void* A)          { ((const CppType*)(A))->~CppType(); }                               \
void  CopyAssign (void* A, void* B) { *static_cast<CppType*>(A) = *static_cast<CppType*>(B); }           \
void  MoveAssign (void* A, void* B) { *static_cast<CppType*>(A) = std::move(*static_cast<CppType*>(B)); }\
