#pragma once
#include "ReflType.h"
#include "ReflProperty.h"

class REFL_API RStruct : public RType
{
public:
    RStruct(const std::string& InName = "")
        : RType(InName)
    {}

    const std::vector<std::unique_ptr<RProperty>>& GetProperties() { return Properties; }

    RProperty* FindPropertyByName(const std::string& Name);

private:
    std::vector<std::unique_ptr<RProperty>>& GetPropertiesPrivate() { return Properties; }
    std::vector<std::unique_ptr<RProperty>> Properties;

public:
    static RStruct* Find(const std::string& StructName) { return static_cast<RStruct*>(RType::Find(StructName)); }
    template<typename T> friend struct TStaticClass;

};

