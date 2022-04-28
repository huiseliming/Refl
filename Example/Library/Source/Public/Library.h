/* REFL_HEADER */

#pragma once
#include "ExampleLibraryExport.h"
#include "Refl.h"



class EXAMPLE_LIBRARY_API RCLASS(Name=Library, lls = "1\"") RLibrary
{
	GENERATED_BODY()
public:


	RPROPERTY()
	std::string LibraryName;
	RPROPERTY()
	uint32_t type;
};















