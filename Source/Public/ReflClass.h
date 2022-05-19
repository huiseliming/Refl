#pragma once
#include "ReflType.h"

class RProperty;
class RFunction;

class REFL_API RClass : public RType
{
public:
    RClass(const std::string& InName = "")
        : RType(InName)
    {}
    ~RClass();
    RClass* GetParentClass() { return static_cast<RClass*>(NextNode); }
    RProperty* GetFirstProperty() { return FirstProperty; }
    RProperty* GetLastProperty();

    void AddProperty(RProperty* InProperty);

    RProperty* FindPropertyByName(const std::string& Name);

protected:
    virtual void Register() override
    {
        RType::Register();
        std::lock_guard<std::mutex> Lock(ClassNameToReflClassUnorderedMapMutex);
        //assert(!ClassNameToReflClassUnorderedMap.contains(GetName()));
        ClassNameToReflClassUnorderedMap.insert(std::make_pair(GetName(), this));
    }

    virtual void Deregister() override
    {
        {
            std::lock_guard<std::mutex> Lock(ClassNameToReflClassUnorderedMapMutex);
            ClassNameToReflClassUnorderedMap.erase(GetName());
        }
        RType::Deregister();
    }

    void SetParentClass(RClass* InParentClass);

private:
    RProperty* FirstProperty;
    RFunction* FirstFunction;
public:
    /**
     *  通过形如ClassName, Namespace::ClassName的类名获取Refl类对象
     */
    static RClass* FindClass(const std::string& ClassName);

private:
    static std::unordered_map<std::string, RClass*> ClassNameToReflClassUnorderedMap;
    static std::mutex ClassNameToReflClassUnorderedMapMutex;
};

template<typename TCppType>
class TReflClass : public RClass
{
public:
    TReflClass(const std::string& InName = "")
        : RClass(InName)
    {}

    REFL_TYPE_OPERATOR_FUNCTION_IMPL(TCppType)
};

