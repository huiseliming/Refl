#pragma once
#include "ReflType.h"
#include "ReflField.h"

class REFL_API RStruct : public RType
{
public:
    RStruct(const std::string& InName = "")
        : RType(InName)
    {}

    const std::vector<std::unique_ptr<RField>>& GetFields() { return Fields; }

    RField* FindFieldByName(const std::string& Name);

private:
    std::vector<std::unique_ptr<RField>>& GetFieldsPrivate() { return Fields; }
    std::vector<std::unique_ptr<RField>> Fields;

public:
    static RStruct* Find(const std::string& StructName) { return static_cast<RStruct*>(RType::Find(StructName)); }
    template<typename T> friend struct TStaticClass;

};

