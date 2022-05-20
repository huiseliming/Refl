#include "ReflJson.h"

void SetJsonHelper(RProperty* Property, void* InInstancePtr, nlohmann::json& InJson)
{
	uint32_t TypeFlag = Property->GetType()->GetTypeFlag();
	switch (TypeFlag)
	{
	case EReflTypeFlag::RTF_Bool:
		InJson = Property->GetBool(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Float:
	case EReflTypeFlag::RTF_Double:
		InJson = Property->GetFloat(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_SInt8:
	case EReflTypeFlag::RTF_SInt16:
	case EReflTypeFlag::RTF_SInt32:
	case EReflTypeFlag::RTF_SInt64:
		InJson = Property->GetSInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_UInt8:
	case EReflTypeFlag::RTF_UInt16:
	case EReflTypeFlag::RTF_UInt32:
	case EReflTypeFlag::RTF_UInt64:
		InJson = Property->GetUInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_String:
		InJson = Property->GetCString(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Enum:
		InJson = Property->GetUInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Class:
		InJson = nlohmann::json::object();
		ReflClassToNlohmannJson(static_cast<RClass*>(Property->GetType()), Property->GetRowPtr(InInstancePtr), InJson);
		break;
	default:
		break;
	}
}

void ReflClassToNlohmannJson(RClass* InReflClass, void* InInstancePtr, nlohmann::json& OutJson)
{
	auto Property = InReflClass->GetFirstProperty();
	while (Property)
	{
		uint32_t TypeFlag = Property->GetType()->GetTypeFlag();
		switch (TypeFlag)
		{
		case EReflTypeFlag::RTF_Bool:
		case EReflTypeFlag::RTF_Float:
		case EReflTypeFlag::RTF_Double:
		case EReflTypeFlag::RTF_SInt8:
		case EReflTypeFlag::RTF_SInt16:
		case EReflTypeFlag::RTF_SInt32:
		case EReflTypeFlag::RTF_SInt64:
		case EReflTypeFlag::RTF_UInt8:
		case EReflTypeFlag::RTF_UInt16:
		case EReflTypeFlag::RTF_UInt32:
		case EReflTypeFlag::RTF_UInt64:
		case EReflTypeFlag::RTF_String:
		case EReflTypeFlag::RTF_Enum:
		case EReflTypeFlag::RTF_Class:
			SetJsonHelper(Property, InInstancePtr, OutJson[Property->GetName()]);
			break;
		case EReflTypeFlag::RTF_Vector:
		{
			OutJson[Property->GetName()] = nlohmann::json::array();
			RVectorProperty* VectorProperty = static_cast<RVectorProperty*>(Property);
			RProperty* ElementProp = VectorProperty->GetElementProp();
			RStdVectorContainer* StdVectorContainer = static_cast<RStdVectorContainer*>(Property->GetType());
			for (size_t i = 0; i < StdVectorContainer->GetSize(Property->GetRowPtr(InInstancePtr)); i++)
			{
				SetJsonHelper(ElementProp, StdVectorContainer->GetElement(Property->GetRowPtr(InInstancePtr), i), OutJson[Property->GetName()][i]);
			}
			break;
		}
		case EReflTypeFlag::RTF_Set:
		{
			OutJson[Property->GetName()] = nlohmann::json::array();
			RSetProperty* SetProperty = static_cast<RSetProperty*>(Property);
			RProperty* ElementProp = SetProperty->GetElementProp();
			RStdSetContainer* StdSetContainer = static_cast<RStdSetContainer*>(Property->GetType());
			StdSetContainer->Foreach(
				Property->GetRowPtr(InInstancePtr),
				[&](void* ElementPtr) {
					OutJson[Property->GetName()].push_back({});
					SetJsonHelper(ElementProp, ElementPtr, OutJson[Property->GetName()].back());
				});
			break;
		}
		default:
			break;
		}
		Property = Property->GetNextProperty();
	}
}

void SetPropertyHelper(RProperty* Property, nlohmann::json& InJson, void* InInstancePtr)
{
	uint32_t TypeFlag = Property->GetType()->GetTypeFlag();
	switch (TypeFlag)
	{
	case EReflTypeFlag::RTF_Bool:
		if (InJson.is_boolean())
		{
			Property->SetBool(InInstancePtr, InJson.get<bool>());
		}
		break;
	case EReflTypeFlag::RTF_Float:
	case EReflTypeFlag::RTF_Double:
		if (InJson.is_number_float())
		{
			Property->SetFloat(InInstancePtr, InJson.get<double>());
		}
		break;
	case EReflTypeFlag::RTF_SInt8:
	case EReflTypeFlag::RTF_SInt16:
	case EReflTypeFlag::RTF_SInt32:
	case EReflTypeFlag::RTF_SInt64:
		if (InJson.is_number_integer())
		{
			Property->SetSInt(InInstancePtr, InJson.get<int64_t>());
		}
		break;
	case EReflTypeFlag::RTF_UInt8:
	case EReflTypeFlag::RTF_UInt16:
	case EReflTypeFlag::RTF_UInt32:
	case EReflTypeFlag::RTF_UInt64:
		if (InJson.is_number_unsigned())
		{
			Property->SetUInt(InInstancePtr, InJson.get<uint64_t>());
		}
		break;
	case EReflTypeFlag::RTF_String:
		if (InJson.is_string())
		{
			Property->SetString(InInstancePtr, InJson.get_ptr<nlohmann::json::string_t*>()->c_str());
		}
		break;
	case EReflTypeFlag::RTF_Enum:
		if (InJson.is_number_unsigned())
		{
			Property->SetUInt(InInstancePtr, InJson.get<uint64_t>());
		}
		if (InJson.is_number_integer())
		{
			Property->SetUInt(InInstancePtr, InJson.get<int64_t>());
		}
		break;
	case EReflTypeFlag::RTF_Class:
		if (InJson.is_object()) NlohmannJsonToReflClass(InJson, static_cast<RClass*>(Property->GetType()), Property->GetRowPtr(InInstancePtr));
		break;
	default:
		break;
	}
}

void NlohmannJsonToReflClass(nlohmann::json& InJson, RClass* ReflClass, void* OutInstancePtr)
{
	auto Property = ReflClass->GetFirstProperty();
	while (Property)
	{
		uint32_t TypeFlag = Property->GetType()->GetTypeFlag();
		switch (TypeFlag)
		{
		case EReflTypeFlag::RTF_Bool:
		case EReflTypeFlag::RTF_Float:
		case EReflTypeFlag::RTF_Double:
		case EReflTypeFlag::RTF_SInt8:
		case EReflTypeFlag::RTF_SInt16:
		case EReflTypeFlag::RTF_SInt32:
		case EReflTypeFlag::RTF_SInt64:
		case EReflTypeFlag::RTF_UInt8:
		case EReflTypeFlag::RTF_UInt16:
		case EReflTypeFlag::RTF_UInt32:
		case EReflTypeFlag::RTF_UInt64:
		case EReflTypeFlag::RTF_String:
		case EReflTypeFlag::RTF_Enum:
		case EReflTypeFlag::RTF_Class:
			SetPropertyHelper(Property, InJson[Property->GetName()], OutInstancePtr);
			//NlohmannJsonToReflClass(InJson[Property->GetName()], static_cast<RClass*>(Property->GetType()), Property->GetRowPtr(OutInstancePtr));
			break;
		case EReflTypeFlag::RTF_Vector:
		{
			if (InJson[Property->GetName()].is_array())
			{
				RVectorProperty* VectorProperty = static_cast<RVectorProperty*>(Property);
				RProperty* ElementProp = VectorProperty->GetElementProp();
				RStdVectorContainer* StdVectorContainer = static_cast<RStdVectorContainer*>(Property->GetType());
				StdVectorContainer->Clear(Property->GetRowPtr(OutInstancePtr));
				StdVectorContainer->Resize(Property->GetRowPtr(OutInstancePtr), InJson[Property->GetName()].size());
				for (size_t i = 0; i < InJson[Property->GetName()].size(); i++)
				{
					SetPropertyHelper(ElementProp, InJson[Property->GetName()][i], StdVectorContainer->GetElement(Property->GetRowPtr(OutInstancePtr), i));
				}
			}
		}
		break;
		case EReflTypeFlag::RTF_Set:
		{
			if (InJson[Property->GetName()].is_array())
			{
				RSetProperty* SetProperty = static_cast<RSetProperty*>(Property);
				RProperty* ElementProp = SetProperty->GetElementProp();
				RStdSetContainer* StdSetContainer = static_cast<RStdSetContainer*>(Property->GetType());
				StdSetContainer->Clear(Property->GetRowPtr(OutInstancePtr));
				for (size_t i = 0; i < InJson[Property->GetName()].size(); i++)
				{
					void* AddRowPtr = StdSetContainer->Add(Property->GetRowPtr(OutInstancePtr));
					SetPropertyHelper(ElementProp,InJson[Property->GetName()][i], AddRowPtr);
				}
			}
			break;
		}
		default:
			break;
		}
		Property = Property->GetNextProperty();
	}
}



