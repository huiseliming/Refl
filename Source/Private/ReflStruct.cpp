#include "ReflStruct.h"
#include "ReflField.h"

RField* RStruct::FindFieldByName(const std::string& InFieldName)
{
    for (auto& Field : Fields)
    {
        if (Field->GetName().size() == InFieldName.size() && Field->GetName() == InFieldName)
        {
            return Field.get();
        }
    }
    return nullptr;
}
