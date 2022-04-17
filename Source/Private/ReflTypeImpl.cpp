#include "ReflTypeImpl.h"

RType* GetStringType()
{
    static RStringType StringType("String");
    return &StringType;;
}

std::unordered_map<std::type_index, RTypePtr>& GetReflTypeTable()
{
    static std::unordered_map<std::type_index, RTypePtr> ReflTypeTable;
    return ReflTypeTable;
}

#define DECL_BUILTIN_TYPE_GET_FUNCTION(BuiltinType, NameString)    \
template<> RType* GetBuiltinType<BuiltinType>()                    \
{                                                                  \
    static TBuiltinType<BuiltinType> NameString##Type(#NameString);\
    return &NameString##Type;                                      \
}

DECL_BUILTIN_TYPE_GET_FUNCTION(bool, Bool)
DECL_BUILTIN_TYPE_GET_FUNCTION(int8_t, SInt8)
DECL_BUILTIN_TYPE_GET_FUNCTION(int16_t, SInt16)
DECL_BUILTIN_TYPE_GET_FUNCTION(int32_t, SInt32)
DECL_BUILTIN_TYPE_GET_FUNCTION(int64_t, SInt64)
DECL_BUILTIN_TYPE_GET_FUNCTION(uint8_t, UInt8)
DECL_BUILTIN_TYPE_GET_FUNCTION(uint16_t, UInt16)
DECL_BUILTIN_TYPE_GET_FUNCTION(uint32_t, UInt32)
DECL_BUILTIN_TYPE_GET_FUNCTION(uint64_t, UInt64)
DECL_BUILTIN_TYPE_GET_FUNCTION(float, Float)
DECL_BUILTIN_TYPE_GET_FUNCTION(double, Double)
