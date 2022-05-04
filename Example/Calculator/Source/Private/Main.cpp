#include <iostream>
#include "Calculator.h"
#include "json.hpp"

int main()
{
	IReflModule::Ref().Initialize();
	//-----------------------------------------------------
	RJsonTest JsonTest;
	{
		nlohmann::json Json;
		ReflClassToNlohmannJson(RJsonTest::StaticClass(), &JsonTest, Json);
		std::cout << Json.dump(2) << std::endl;
	}
	{
		nlohmann::json Json;
		ReflClassToNlohmannJson(JsonTest, Json);
		std::cout << Json.dump(2) << std::endl;
	}
	//-----------------------------------------------------
	RClass* CalculatorClass = RCalculator::StaticClass();
	RCalculator* Calculator = static_cast<RCalculator*>(CalculatorClass->New());
	Calculator->Vectors.push_back(RVecotr3D());
	Calculator->Vectors.push_back(RVecotr3D());
	Calculator->Vectors.push_back(RVecotr3D());
	Calculator->VectorMathOperator = EVectorMathOperator::VMO_Add;
	Calculator->VectorSet.insert("A");
	for (auto& Vector : Calculator->Vectors)
	{
		Vector.X = 1;
		Vector.Y = 2;
		Vector.Z = 3;
	}
	nlohmann::json CalculatorJsonData;
	ReflClassToNlohmannJson(Calculator, CalculatorJsonData);
	CalculatorJsonData["VectorMathOperator"] = EVectorMathOperator::VMO_Mul;
	std::cout << CalculatorJsonData.dump(2) << std::endl;
	NlohmannJsonToReflClass(CalculatorJsonData, Calculator);
	if (Calculator->VectorMathOperator == EVectorMathOperator::VMO_Add)
	{
		Calculator->Result.X = 0.f;
		Calculator->Result.Y = 0.f;
		Calculator->Result.Z = 0.f;
	}
	else
	{
		Calculator->Result.X = 1.f;
		Calculator->Result.Y = 1.f;
		Calculator->Result.Z = 1.f;
	}
	for (auto& Vector : Calculator->Vectors)
	{
		if (Calculator->VectorMathOperator == EVectorMathOperator::VMO_Add)
		{
			Calculator->Result.X += Vector.X;
			Calculator->Result.Y += Vector.Y;
			Calculator->Result.Z += Vector.Z;
		}
		else
		{
			Calculator->Result.X *= Vector.X;
			Calculator->Result.Y *= Vector.Y;
			Calculator->Result.Z *= Vector.Z;
		}
	}
	ReflClassToNlohmannJson(Calculator, CalculatorJsonData);
	std::cout << CalculatorJsonData.dump(2) << std::endl;

	//---------------------------------------------------------
	GLOG(Debug, "??????????");
	PostTask(TI_Main, [](int cc) { GLOG(Debug, "??????{:d}", cc); }, 1);
	PostTask<TI_Main>([](int cc) { GLOG(Debug, "??????{:d}", cc); }, 3);
	while (true)
	{
		GMainThreadQueue->ProcessTask();
	}

	//std::vector<RVecotr3D>StdVector;
	//RVecotr3D ElementA, ElementB;
	//if (ElementA == ElementB)
	//{

	//}
	//for (size_t i = 0; i < StdVector.size(); i++)
	//{
	//	if (StdVector[i] == Element)
	//	{
	//		return static_cast<int32_t>(i);
	//	}
	//}
	//return InvalidIndex;

	return 0;
}
