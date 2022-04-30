
#include "Executable.h"




int main() 
{
	RClass* ExecutableClass = RExecutable::StaticClass();
	RExecutable* Executable = static_cast<RExecutable*>(ExecutableClass->New());
	Executable->ExecutableName = "ExampleExecutable";
	RClass* ExampleLibraryClass = RClass::FindClass("ExampleLibrary");
	Executable->LinkedLibrary.insert(std::make_pair("ExampleLibrary", *(RLibrary*)ExampleLibraryClass->New()));

	return 0;
}