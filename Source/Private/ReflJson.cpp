#include "ReflJson.h"

void SetJsonHelper(RProperty* Property, void* InInstancePtr, nlohmann::json& OutJson)
{
	uint32_t TypeFlag = Property->GetType()->GetTypeFlag();
	switch (TypeFlag)
	{
	case EReflTypeFlag::RTF_Bool:
		OutJson = Property->GetBool(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Float:
	case EReflTypeFlag::RTF_Double:
		OutJson = Property->GetFloat(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_SInt8:
	case EReflTypeFlag::RTF_SInt16:
	case EReflTypeFlag::RTF_SInt32:
	case EReflTypeFlag::RTF_SInt64:
		OutJson = Property->GetSInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_UInt8:
	case EReflTypeFlag::RTF_UInt16:
	case EReflTypeFlag::RTF_UInt32:
	case EReflTypeFlag::RTF_UInt64:
		OutJson = Property->GetUInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_String:
		OutJson = Property->GetCString(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Enum:
		//OutJson[Property->GetName()] = Property->GetUInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Class:
		OutJson = nlohmann::json::object();
		ReflClassToNlohmannJson(static_cast<RClass*>(Property->GetType()), Property->GetRowPtr(InInstancePtr), OutJson);
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
		}
		break;
		default:
			break;
		}
		Property = Property->GetNextProperty();
	}
}

void SetPropertyHelper(RProperty* Property, void* InInstancePtr, nlohmann::json& OutJson)
{
	uint32_t TypeFlag = Property->GetType()->GetTypeFlag();
	switch (TypeFlag)
	{
	case EReflTypeFlag::RTF_Bool:
		if (OutJson.is_boolean())
		{
			Property->SetBool(InInstancePtr, OutJson.get<bool>());
		}
		break;
	case EReflTypeFlag::RTF_Float:
	case EReflTypeFlag::RTF_Double:
		if (OutJson.is_number_float())
		{
			Property->SetFloat(InInstancePtr, OutJson.get<double>());
		}
		break;
	case EReflTypeFlag::RTF_SInt8:
	case EReflTypeFlag::RTF_SInt16:
	case EReflTypeFlag::RTF_SInt32:
	case EReflTypeFlag::RTF_SInt64:
		if (OutJson.is_number_integer())
		{
			Property->SetSInt(InInstancePtr, OutJson.get<int64_t>());
		}
		break;
	case EReflTypeFlag::RTF_UInt8:
	case EReflTypeFlag::RTF_UInt16:
	case EReflTypeFlag::RTF_UInt32:
	case EReflTypeFlag::RTF_UInt64:
		if (OutJson.is_number_integer())
		{
			Property->SetUInt(InInstancePtr, OutJson.get<uint64_t>());
		}
		break;
	case EReflTypeFlag::RTF_String:
		if (OutJson.is_number_integer())
		{
			Property->SetString(InInstancePtr, OutJson.get_ptr<nlohmann::json::string_t*>()->c_str());
		}
		break;
	case EReflTypeFlag::RTF_Enum:
		//OutJson = Property->GetUInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Class:
		NlohmannJsonToReflClass(OutJson, static_cast<RClass*>(Property->GetType()), Property->GetRowPtr(InInstancePtr));
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
			SetPropertyHelper(Property, OutInstancePtr, InJson[Property->GetName()]);
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
					SetPropertyHelper(ElementProp, StdVectorContainer->GetElement(Property->GetRowPtr(OutInstancePtr), i), InJson[Property->GetName()][i]);
				}
			}
		}
		break;
		default:
			break;
		}
		Property = Property->GetNextProperty();
	}
}



