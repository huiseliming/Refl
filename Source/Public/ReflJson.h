#pragma once
#include "json.hpp"
#include "ReflProperty.h"
#include "ReflClass.h"

REFL_API void SetJsonHelper(RProperty* Property, void* InInstancePtr, nlohmann::json& InJson);

REFL_API void SetPropertyHelper(RProperty* Property,nlohmann::json& InJson, void* InInstancePtr);

REFL_API void ReflClassToNlohmannJson(RClass* InReflClass, void* InInstancePtr, nlohmann::json& OutJson);

REFL_API void NlohmannJsonToReflClass(nlohmann::json& InJson, RClass* ReflClass, void* OutInstancePtr);

template<typename T>
void ReflClassToNlohmannJson(T& Ref, nlohmann::json& OutJson)
{
	ReflClassToNlohmannJson(T::StaticClass(), &Ref, OutJson);
}

template<typename T>
void ReflClassToNlohmannJson(T* Ptr, nlohmann::json& OutJson)
{
	ReflClassToNlohmannJson(T::StaticClass(), Ptr, OutJson);
}

template<typename T>
void NlohmannJsonToReflClass(nlohmann::json& InJson, T& Ref)
{
	NlohmannJsonToReflClass(InJson, T::StaticClass(), &Ref);
}

template<typename T>
void NlohmannJsonToReflClass(nlohmann::json& InJson, T* Ptr)
{
	NlohmannJsonToReflClass(InJson, T::StaticClass(), Ptr);
}

