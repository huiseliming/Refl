#pragma once
#include "ReflCore.h"

struct REFL_API IReflModule
{
    static IReflModule& Ref();
    /**
     * 使用Refl之前需要先在主线程进行初始化
     */
    void Initialize();

    /**
     * 程序退出之前需要为Refl执行清理
     */
    void Deinitialize();

    bool IsMainThread();

    std::thread::id MainThreadId;
};
