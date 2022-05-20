#include "ReflClass.h"
#include "ReflProperty.h"

std::unordered_map<std::string, RClass*> RClass::ClassNameToReflClassUnorderedMap;
std::mutex RClass::ClassNameToReflClassUnorderedMapMutex;

RClass::~RClass()
{
    RProperty* NextPropertyOfLastProperty = GetParentClass() ? GetParentClass()->FirstProperty : nullptr;
    if (NextPropertyOfLastProperty != FirstProperty)
    {
        TRecordIterator<RProperty> PropertyIterator(FirstProperty);
        while (PropertyIterator)
        {
            RProperty* DeleteTemp = PropertyIterator.Ptr();
            RProperty* NextProperty = PropertyIterator.Ptr()->GetNextProperty();
            ++PropertyIterator;
            delete DeleteTemp;
            if (NextProperty == NextPropertyOfLastProperty) break;
        }
    }
    FirstProperty = nullptr;
}

RProperty* RClass::GetLastProperty()
{
    RProperty* NextPropertyOfLastProperty = GetParentClass() ? GetParentClass()->FirstProperty : nullptr;
    if (NextPropertyOfLastProperty == FirstProperty) return nullptr;
    TRecordIterator<RProperty> PropertyIterator(FirstProperty);
    while (PropertyIterator)
    {
        if (PropertyIterator.Ptr()->GetNextProperty() == NextPropertyOfLastProperty)
        {
            break;
        }
        ++PropertyIterator;
    }
    return PropertyIterator.Ptr();
}

void RClass::AddProperty(RProperty* InProperty)
{
    if (RProperty* LastProperty = GetLastProperty())
    {
        InProperty->SetNextProperty(LastProperty->GetNextProperty());
        LastProperty->SetNextProperty(InProperty);
    }
    else
    {
        FirstProperty = InProperty;
        if (GetParentClass())
        {
            FirstProperty->SetNextProperty(GetParentClass()->GetFirstProperty());
        }
    }
}

RProperty* RClass::FindPropertyByName(const std::string& Name)
{
    RProperty* Prop = GetFirstProperty();
    while (Prop)
    {
        if (Prop->GetName() == Name)
        {
            break;
        }
        Prop = Prop->GetNextProperty();
    }
    return Prop;
}

void RClass::SetParentClass(RClass* InParentClass)
{
    // 在设置父类之前先设置属性链
    if (RProperty* LastProperty = GetLastProperty())
    {
        LastProperty->SetNextProperty(InParentClass->GetFirstProperty());
    }
    else
    {
        FirstProperty = InParentClass->GetFirstProperty();
    }
    NextNode = InParentClass;
}

RClass* RClass::FindClass(const std::string& ClassName)
{
	std::lock_guard<std::mutex> Lock(ClassNameToReflClassUnorderedMapMutex);
	auto It = ClassNameToReflClassUnorderedMap.find(ClassName);
	if (It != ClassNameToReflClassUnorderedMap.end())
		return It->second;
    return nullptr;
}
