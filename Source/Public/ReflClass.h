#pragma once
#include "ReflStruct.h"
#include "ReflFunction.h"

class REFL_API RClass : public RStruct
{
public:
    RClass(const std::string& InName = "")
        : RStruct(InName)
    {}
    ~RClass() = default;

protected:

private:
    //std::vector<std::unique_ptr<RFunction>> Functions;

public:
    static RClass* Find(const std::string& ClassName) { return static_cast<RClass*>(RStruct::Find(ClassName)); }
    template<typename T> friend struct TStaticClass;

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

