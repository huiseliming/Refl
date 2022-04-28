/* REFL_HEADER */
#pragma once
#include "Refl.h"
#include "Library.h"


class RCLASS() RExecutable
{
	GENERATED_BODY()
public:


	RPROPERTY()
	std::map<std::string, RLibrary> LinkedLibrary;

	RPROPERTY()
	std::string ExecutableName;
};


