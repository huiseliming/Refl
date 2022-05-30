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

typedef int* intptr;
 
class TOOL_API RCLASS(Name=Vector) RVecotr3D
{
	GENERATED_BODY()
public:

	RFIELD()
	float X = 0.f; 
	RFIELD()
	float Y = 0.f;
	RFIELD() 
	float Z = 0.f;
	 
	RFUNCTION() 
	int Add(int a, int b);
	     
}; 
 
 


  

 


 


