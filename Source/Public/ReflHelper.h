#pragma once
#include "CppStandardLibrary.h"
#include "ReflRecord.h"

inline void ReflSetMetadata(RRecord* Record, const std::unordered_map<std::string, std::string>& InMetadata)
{
    Record->SetMetadata(InMetadata);
}
