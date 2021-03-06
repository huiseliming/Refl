/* REFL_HEADER */

#pragma once
#include "ToolExport.h"
#include "Refl.h"

enum RENUM(Name=VectorMathOperator) EVectorMathOperator
{
	VMO_Add RMETADATA(DisplayName = "Add"),
	VMO_Mul RMETADATA(DisplayName = "Mul"),
}; 

#include "Tool.generated.h"

class TOOL_API RCLASS(Name=Vector) RVecotr3D
{
	GENERATED_BODY()
public:

	RPROPERTY()
	float X = 0.f;
	RPROPERTY()
	float Y = 0.f;
	RPROPERTY()
	float Z = 0.f;
};













