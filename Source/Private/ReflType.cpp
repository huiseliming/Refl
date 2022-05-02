#include "ReflType.h"

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

#define GET_BUILTIN_TYPE_FUNCTION_IMPL(BuiltinType, NameString)    \
template<> RType* GetBuiltinType<BuiltinType>()                    \
{                                                                  \
    static TBuiltinType<BuiltinType> NameString##Type(#NameString);\
    return &NameString##Type;                                      \
}

GET_BUILTIN_TYPE_FUNCTION_IMPL(bool, Bool)
GET_BUILTIN_TYPE_FUNCTION_IMPL(int8_t, SInt8)
GET_BUILTIN_TYPE_FUNCTION_IMPL(int16_t, SInt16)
GET_BUILTIN_TYPE_FUNCTION_IMPL(int32_t, SInt32)
GET_BUILTIN_TYPE_FUNCTION_IMPL(int64_t, SInt64)
GET_BUILTIN_TYPE_FUNCTION_IMPL(uint8_t, UInt8)
GET_BUILTIN_TYPE_FUNCTION_IMPL(uint16_t, UInt16)
GET_BUILTIN_TYPE_FUNCTION_IMPL(uint32_t, UInt32)
GET_BUILTIN_TYPE_FUNCTION_IMPL(uint64_t, UInt64)
GET_BUILTIN_TYPE_FUNCTION_IMPL(float, Float)
GET_BUILTIN_TYPE_FUNCTION_IMPL(double, Double)
