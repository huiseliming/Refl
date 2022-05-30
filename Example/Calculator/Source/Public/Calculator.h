/* REFL_HEADER */
#pragma once
#include "Refl.h"
#include "Tool.h"
#include "Calculator.generated.h"
 
class RCLASS() RCalculator
{
	GENERATED_BODY()
public:


	//RFIELD()
	//std::set<std::string> VectorMap;
	RFIELD()
	std::set<std::string> VectorSet;
	RFIELD()
	std::vector<RVecotr3D> Vectors;
	RFIELD()
	EVectorMathOperator VectorMathOperator = EVectorMathOperator::VMO_Add;
	RFIELD()
	RVecotr3D Result;

};


struct RCLASS() RSubJsonTest
{
	GENERATED_BODY()
public:
	RFIELD()
	std::string String = "RSubJsonTest";
	RFIELD()
	bool Bool = false;
	RFIELD()
	float Float = 10.1f;
	RFIELD()
	int32_t Int32 = 110;
};

struct RCLASS() RJsonTest
{
	GENERATED_BODY()
public:
	RFIELD()
	bool True = true;
    RFIELD()
	bool False = false;
    RFIELD()
	float Float = 10.01f;
    RFIELD()
	double Double = 100.001f;
    RFIELD()
	uint8_t UInt8= 10;
    RFIELD()
	int8_t Int8= -10;
    RFIELD()
	uint16_t UInt16 = 100;
    RFIELD()
	int16_t Int16 = -100;
    RFIELD()
	uint32_t UInt32 = 1000;
    RFIELD()
	int32_t Int32 = -1000;
    RFIELD()
	uint64_t UInt64 = 10000;
    RFIELD()
	int64_t Int64 = -10000;
    RFIELD()
	std::string String = "String";
	RFIELD()
	RSubJsonTest SubJsonTest;
};