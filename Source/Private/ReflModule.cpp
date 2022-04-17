#include "ReflModule.h"

IReflModule& IReflModule::Ref()
{
    static IReflModule ReflModule;
    return ReflModule;
}

void IReflModule::Initialize()
{
    MainThreadId = std::this_thread::get_id();
}

void IReflModule::Deinitialize()
{
    assert(IsMainThread());
}

bool IReflModule::IsMainThread()
{
    return MainThreadId == std::this_thread::get_id();
}
