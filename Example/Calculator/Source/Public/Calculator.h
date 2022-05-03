/* REFL_HEADER */
#pragma once
#include "Refl.h"
#include "Tool.h"
#include "Calculator.generated.h"

class RCLASS() RCalculator
{
	GENERATED_BODY()
public:


	//RPROPERTY()
	//std::set<std::string> VectorMap;
	RPROPERTY()
	std::set<std::string> VectorSet;
	RPROPERTY()
	std::vector<RVecotr3D> Vectors;
	RPROPERTY()
	EVectorMathOperator VectorMathOperator = EVectorMathOperator::VMO_Add;
	RPROPERTY()
	RVecotr3D Result;

};


struct RCLASS() RSubJsonTest
{
	GENERATED_BODY()
public:
	RPROPERTY()
	std::string String = "RSubJsonTest";
	RPROPERTY()
	bool Bool = false;
	RPROPERTY()
	float Float = 10.1f;
	RPROPERTY()
	int32_t Int32 = 110;
};

struct RCLASS() RJsonTest
{
	GENERATED_BODY()
public:
	RPROPERTY()
	bool True = true;
    RPROPERTY()
	bool False = false;
    RPROPERTY()
	float Float = 10.01f;
    RPROPERTY()
	double Double = 100.001f;
    RPROPERTY()
	uint8_t UInt8= 10;
    RPROPERTY()
	int8_t Int8= -10;
    RPROPERTY()
	uint16_t UInt16 = 100;
    RPROPERTY()
	int16_t Int16 = -100;
    RPROPERTY()
	uint32_t UInt32 = 1000;
    RPROPERTY()
	int32_t Int32 = -1000;
    RPROPERTY()
	uint64_t UInt64 = 10000;
    RPROPERTY()
	int64_t Int64 = -10000;
    RPROPERTY()
	std::string String = "String";
	RPROPERTY()
	RSubJsonTest SubJsonTest;
};