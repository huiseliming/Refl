#pragma once
#include "ReflStruct.h"

class RFunction : public RStruct
{
public:
	RFunction(const std::string& Name)
		: RStruct(Name)
	{}

    void Invoke(void* Object, void* StackFrame)
    {
        VMFunction(Object, StackFrame);
    }

protected:
    std::function<void(void*, void*)> VMFunction;

public:
    template<typename T> friend struct TStaticClass;
};

//template<typename AT>
//void UnpackParameters(std::vector<RType*>& OutParameters, AT T)
//{
//	OutParameters.push_back(GetReflType<AT>());
//}
//
//template<typename AT, typename ... ATN>
//void UnpackParameters(std::vector<RType*>& OutParameters, AT T, ATN ... TN)
//{
//	OutParameters.push_back(GetReflType<AT>());
//	UnpackParameters(std::vector<RType*>&OutParameters, TN...);
//}
// 
//template<typename T>
//std::vector<T> UnpackParameters(std::vector<T> N)
//{
//	return N;
//}



template<typename TCppType>
class TFunction : public RFunction
{
public:
	TFunction(const std::string& Name)
		: RFunction(Name)
	{}


    REFL_TYPE_OPERATOR_FUNCTION_IMPL(TCppType)
};

    //struct CFrame : public CFunction::CFrame
    //{
    //    friend class TCppFunction<Ret, Args...>;
    //private:
    //    template<typename T>
    //    struct TArgument
    //    {};


    //    template<typename T>
    //    void NewArgsBuffers(size_t index, TArgument<T> argument)
    //    {
    //        if      constexpr (std::is_rvalue_reference_v<T>) ArgsPtr[index] = new std::remove_reference_t<T>();
    //        else if constexpr (std::is_lvalue_reference_v<T>) ArgsPtr[index] = new std::reference_wrapper<std::remove_reference_t<T>>();
    //        else if constexpr (std::is_pointer_v<T>) (void)0;
    //        else                                              ArgsPtr[index] = new T();
    //    }

    //    template<typename T, typename ...Arguments>
    //    void NewArgsBuffers(size_t index, TArgument<T> argument, TArgument<Arguments>... arguments)
    //    {
    //        if      constexpr (std::is_rvalue_reference_v<T>) ArgsPtr[index] = new std::remove_reference_t<T>();
    //        else if constexpr (std::is_lvalue_reference_v<T>) ArgsPtr[index] = new std::reference_wrapper<std::remove_reference_t<T>>();
    //        else if constexpr (std::is_pointer_v<T>) (void)0;
    //        else                                              ArgsPtr[index] = new T();
    //        NewArgsBuffers(index + 1, arguments...);
    //    }

    //    template<typename T>
    //    void FreeArgsBuffers(size_t index, TArgument<T> argument)
    //    {
    //        if      constexpr (std::is_rvalue_reference_v<T>) delete (std::remove_reference_t<T>*)(ArgsPtr[index]);
    //        else if constexpr (std::is_lvalue_reference_v<T>) delete (std::reference_wrapper<std::remove_reference_t<T>>*)ArgsPtr[index];
    //        else if constexpr (std::is_pointer_v<T>) (void)0;
    //        else                                              delete (T*)(ArgsPtr[index]);
    //    }

    //    template<typename T, typename ...Arguments>
    //    void FreeArgsBuffers(size_t index, TArgument<T> argument, TArgument<Arguments>... arguments)
    //    {
    //        if      constexpr (std::is_rvalue_reference_v<T>) delete (std::remove_reference_t<T>*)(ArgsPtr[index]);
    //        else if constexpr (std::is_lvalue_reference_v<T>) delete (std::reference_wrapper<std::remove_reference_t<T>>*)ArgsPtr[index];
    //        else if constexpr (std::is_pointer_v<T>) (void)0;
    //        else                                              delete (T*)(ArgsPtr[index]);
    //        FreeArgsBuffers(index + 1, arguments...);
    //    }

    //    template<typename T>
    //    std::tuple<T> MakeArgsTuple(size_t index, TArgument<T> argument)
    //    {
    //        if      constexpr (std::is_rvalue_reference_v<T>) return std::tuple<T>(*(std::remove_reference_t<T>*)ArgsPtr[index]);
    //        else if constexpr (std::is_lvalue_reference_v<T>) return std::tuple<T>(((std::reference_wrapper<std::remove_reference_t<T>>*)ArgsPtr[index])->get());
    //        else if constexpr (std::is_pointer_v<T>) return std::tuple<T>((T)ArgsPtr[index]);
    //        else                                              return std::tuple<T>(*(T*)ArgsPtr[index]);
    //    }

    //    template<typename T, typename ...Arguments>
    //    std::tuple<T, Arguments...> MakeArgsTuple(size_t index, TArgument<T> argument, TArgument<Arguments>... arguments)
    //    {
    //        if      constexpr (std::is_rvalue_reference_v<T>) return std::tuple_cat(std::tuple<T>(*(std::remove_reference_t<T>*)ArgsPtr[index]), MakeArgsTuple(index + 1, arguments...));
    //        else if constexpr (std::is_lvalue_reference_v<T>) return std::tuple_cat(std::tuple<T>(((std::reference_wrapper<std::remove_reference_t<T>>*)ArgsPtr[index])->get()), MakeArgsTuple(index + 1, arguments...));
    //        else if constexpr (std::is_pointer_v<T>) return std::tuple_cat(std::tuple<T>((T)ArgsPtr[index]), MakeArgsTuple(index + 1, arguments...));
    //        else                                              return std::tuple_cat(std::tuple<T>(*(T*)ArgsPtr[index]), MakeArgsTuple(index + 1, arguments...));
    //    }

    //public:
    //    CFrame()
    //        : CFunction::CFrame()
    //    {
    //        NewArgsBuffers(0, TArgument<Args>()...);
    //        if constexpr (!std::is_pointer_v<Ret> && !std::is_reference_v<Ret>) RetPtr = new Ret();
    //    }

    //    ~CFrame()
    //    {
    //        FreeArgsBuffers(0, TArgument<Args>()...);
    //        if constexpr (!std::is_pointer_v<Ret> && !std::is_reference_v<Ret>) delete (Ret*)RetPtr;
    //    }

    //    virtual void* GetReturnValuePtr() override { return RetPtr; }
    //    virtual void* GetArgumentPtr(uint32_t index) override { return ArgsPtr[index]; }

    //private:
    //    void* RetPtr;
    //    void* ArgsPtr[sizeof...(Args)];

    //};