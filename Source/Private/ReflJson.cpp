#include "ReflJson.h"

void SetJsonHelper(RField* Field, void* InInstancePtr, nlohmann::json& InJson)
{
	uint32_t TypeFlag = Field->GetType()->GetTypeFlag();
	switch (TypeFlag)
	{
	case EReflTypeFlag::RTF_Bool:
		InJson = Field->GetBool(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Float:
	case EReflTypeFlag::RTF_Double:
		InJson = Field->GetFloat(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_SInt8:
	case EReflTypeFlag::RTF_SInt16:
	case EReflTypeFlag::RTF_SInt32:
	case EReflTypeFlag::RTF_SInt64:
		InJson = Field->GetSInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_UInt8:
	case EReflTypeFlag::RTF_UInt16:
	case EReflTypeFlag::RTF_UInt32:
	case EReflTypeFlag::RTF_UInt64:
		InJson = Field->GetUInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_String:
		InJson = Field->GetCString(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Enum:
		InJson = Field->GetUInt(InInstancePtr);
		break;
	case EReflTypeFlag::RTF_Class:
		InJson = nlohmann::json::object();
		ReflClassToNlohmannJson(static_cast<RClass*>(Field->GetType()), Field->GetRowPtr(InInstancePtr), InJson);
		break;
	default:
		break;
	}
}

void ReflClassToNlohmannJson(RClass* InReflClass, void* InInstancePtr, nlohmann::json& OutJson)
{
	RStruct* ReflStruct = InReflClass;
	while (ReflStruct)
	{
		for (auto& StructField : ReflStruct->GetFields())
		{
			auto Field = StructField.get();
			uint32_t TypeFlag = Field->GetType()->GetTypeFlag();
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
				SetJsonHelper(Field, InInstancePtr, OutJson[Field->GetName()]);
				break;
			case EReflTypeFlag::RTF_Vector:
			{
				OutJson[Field->GetName()] = nlohmann::json::array();
				RVectorField* VectorField = static_cast<RVectorField*>(Field);
				RField* ElementField = VectorField->GetElementField();
				RStdVectorContainer* StdVectorContainer = static_cast<RStdVectorContainer*>(Field->GetType());
				for (size_t i = 0; i < StdVectorContainer->GetSize(Field->GetRowPtr(InInstancePtr)); i++)
				{
					SetJsonHelper(ElementField, StdVectorContainer->GetElement(Field->GetRowPtr(InInstancePtr), i), OutJson[Field->GetName()][i]);
				}
				break;
			}
			case EReflTypeFlag::RTF_Set:
			{
				OutJson[Field->GetName()] = nlohmann::json::array();
				RSetField* SetField = static_cast<RSetField*>(Field);
				RField* ElementField = SetField->GetElementField();
				RStdSetContainer* StdSetContainer = static_cast<RStdSetContainer*>(Field->GetType());
				StdSetContainer->Foreach(
					Field->GetRowPtr(InInstancePtr),
					[&](void* ElementPtr) {
						OutJson[Field->GetName()].push_back({});
						SetJsonHelper(ElementField, ElementPtr, OutJson[Field->GetName()].back());
					});
				break;
			}
			default:
				break;
			}
		}
		ReflStruct = static_cast<RStruct*>(ReflStruct->GetParentType());
	}
}

void SetFieldHelper(RField* Field, nlohmann::json& InJson, void* InInstancePtr)
{
	uint32_t TypeFlag = Field->GetType()->GetTypeFlag();
	switch (TypeFlag)
	{
	case EReflTypeFlag::RTF_Bool:
		if (InJson.is_boolean())
		{
			Field->SetBool(InInstancePtr, InJson.get<bool>());
		}
		break;
	case EReflTypeFlag::RTF_Float:
	case EReflTypeFlag::RTF_Double:
		if (InJson.is_number_float())
		{
			Field->SetFloat(InInstancePtr, InJson.get<double>());
		}
		break;
	case EReflTypeFlag::RTF_SInt8:
	case EReflTypeFlag::RTF_SInt16:
	case EReflTypeFlag::RTF_SInt32:
	case EReflTypeFlag::RTF_SInt64:
		if (InJson.is_number_integer())
		{
			Field->SetSInt(InInstancePtr, InJson.get<int64_t>());
		}
		break;
	case EReflTypeFlag::RTF_UInt8:
	case EReflTypeFlag::RTF_UInt16:
	case EReflTypeFlag::RTF_UInt32:
	case EReflTypeFlag::RTF_UInt64:
		if (InJson.is_number_unsigned())
		{
			Field->SetUInt(InInstancePtr, InJson.get<uint64_t>());
		}
		break;
	case EReflTypeFlag::RTF_String:
		if (InJson.is_string())
		{
			Field->SetString(InInstancePtr, InJson.get_ptr<nlohmann::json::string_t*>()->c_str());
		}
		break;
	case EReflTypeFlag::RTF_Enum:
		if (InJson.is_number_unsigned())
		{
			Field->SetUInt(InInstancePtr, InJson.get<uint64_t>());
		}
		if (InJson.is_number_integer())
		{
			Field->SetUInt(InInstancePtr, InJson.get<int64_t>());
		}
		break;
	case EReflTypeFlag::RTF_Class:
		if (InJson.is_object()) NlohmannJsonToReflClass(InJson, static_cast<RClass*>(Field->GetType()), Field->GetRowPtr(InInstancePtr));
		break;
	default:
		break;
	}
}

void NlohmannJsonToReflClass(nlohmann::json& InJson, RClass* InReflClass, void* OutInstancePtr)
{
	RStruct* ReflStruct = InReflClass;
	while (ReflStruct)
	{
		for (auto& StructField : ReflStruct->GetFields())
		{
			auto Field = StructField.get();
			uint32_t TypeFlag = Field->GetType()->GetTypeFlag();
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
				SetFieldHelper(Field, InJson[Field->GetName()], OutInstancePtr);
				//NlohmannJsonToReflClass(InJson[Field->GetName()], static_cast<RClass*>(Field->GetType()), Field->GetRowPtr(OutInstancePtr));
				break;
			case EReflTypeFlag::RTF_Vector:
			{
				if (InJson[Field->GetName()].is_array())
				{
					RVectorField* VectorField = static_cast<RVectorField*>(Field);
					RField* ElementField = VectorField->GetElementField();
					RStdVectorContainer* StdVectorContainer = static_cast<RStdVectorContainer*>(Field->GetType());
					StdVectorContainer->Clear(Field->GetRowPtr(OutInstancePtr));
					StdVectorContainer->Resize(Field->GetRowPtr(OutInstancePtr), InJson[Field->GetName()].size());
					for (size_t i = 0; i < InJson[Field->GetName()].size(); i++)
					{
						SetFieldHelper(ElementField, InJson[Field->GetName()][i], StdVectorContainer->GetElement(Field->GetRowPtr(OutInstancePtr), i));
					}
				}
			}
			break;
			case EReflTypeFlag::RTF_Set:
			{
				if (InJson[Field->GetName()].is_array())
				{
					RSetField* SetField = static_cast<RSetField*>(Field);
					RField* ElementField = SetField->GetElementField();
					RStdSetContainer* StdSetContainer = static_cast<RStdSetContainer*>(Field->GetType());
					StdSetContainer->Clear(Field->GetRowPtr(OutInstancePtr));
					for (size_t i = 0; i < InJson[Field->GetName()].size(); i++)
					{
						void* AddRowPtr = StdSetContainer->Add(Field->GetRowPtr(OutInstancePtr));
						SetFieldHelper(ElementField, InJson[Field->GetName()][i], AddRowPtr);
					}
				}
				break;
			}
			default:
				break;
			}
		}		
		ReflStruct = static_cast<RStruct*>(ReflStruct->GetParentType());
	}
}



