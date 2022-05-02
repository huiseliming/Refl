#pragma once

#include "ReflType.h"

class REnum : public RType
{
public:
    REnum(const std::string& InName)
        : RType(InName)
    {}


    virtual const std::string& GetDisplayNameByUInt64Value(uint64_t Value) = 0;
    virtual const std::string& GetDisplayNameByIndex(uint64_t Index) = 0;
    virtual uint64_t GetUInt64ValueByDisplayName(const std::string& DisplayName) = 0;
    virtual uint64_t GetUInt64ValueByIndex(uint64_t Index) = 0;
};


template<typename T>
class TEnum : public REnum
{
    static_assert(std::is_enum_v<T>);
public:
    TEnum(const std::string& InName)
        : REnum(InName)
    {}

    virtual const std::string& GetDisplayNameByUInt64Value(uint64_t Value) override
    {
        T EnumValue = static_cast<T>(Value);
        return GetDisplayNameByValuePrivate(EnumValue);
    }

    virtual const std::string& GetDisplayNameByIndex(uint64_t Index) override
    {
        return GetDisplayNameByIndexPrivate(Index);
    }

    virtual uint64_t GetUInt64ValueByDisplayName(const std::string& DisplayName) override
    {
        return static_cast<uint64_t>(GetValueByDisplayNamePrivate(DisplayName));
    }

    virtual uint64_t GetUInt64ValueByIndex(uint64_t Index) override
    {
        return static_cast<uint64_t>(GetValueByIndexPrivate(Index));
    }

protected:
    template<class T> friend struct TStaticEnum;
    void AddEnumValue(T Value, const std::string& DisplayName)
    {
        auto Index = EnumPairs.size();
        EnumPairs.push_back(std::make_pair(Value, DisplayName));
        ValueToIndex.insert(std::make_pair(Value, Index));
        DisplayNameToIndex.insert(std::make_pair(DisplayName, Index));
    }

private:
    const std::string& GetDisplayNameByValuePrivate(T& Value) 
    {
        auto It = ValueToIndex.find(Value);
        if (It != ValueToIndex.end()) return EnumPairs[It->second].second;
        return IStaticVariable::EmptyString;
    }

    const std::string& GetDisplayNameByIndexPrivate(uint64_t Index)
    {
        if (Index < EnumPairs.size())
            return EnumPairs[Index].second;
        return IStaticVariable::EmptyString;
    }

    const T& GetValueByDisplayNamePrivate(const std::string& DisplayName)
    {
        auto It = DisplayNameToIndex.find(DisplayName);
        if (It != DisplayNameToIndex.end()) return EnumPairs[It->second].first;
        return T(0);
    }

    const T& GetValueByIndexPrivate(uint64_t Index)
    {
        if (Index < EnumPairs.size())
            return EnumPairs[Index].first;
        return T(0);
    }

    std::map<T, uint64_t> ValueToIndex;
    std::unordered_map<std::string, uint64_t> DisplayNameToIndex;
    std::vector<std::pair<T, std::string>> EnumPairs;
    
    REFL_TYPE_OPERATOR_FUNCTION_IMPL(T)
};
