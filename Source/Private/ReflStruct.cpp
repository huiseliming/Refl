#include "ReflStruct.h"
#include "ReflProperty.h"

RProperty* RStruct::FindPropertyByName(const std::string& InPropertyName)
{
    for (auto& Property : Properties)
    {
        if (Property->GetName().size() == InPropertyName.size() && Property->GetName() == InPropertyName)
        {
            return Property.get();
        }
    }
    return nullptr;
}
