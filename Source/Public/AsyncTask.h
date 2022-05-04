#pragma once
#include "Queue.h"
#include "CppStandardLibrary.h"


enum EThreadId
{
	TI_Main,
	TI_TaskMin = 0x80000000,
	TI_TaskMax = 0x800000FF,
};

using FTask = std::function<void()>;
using FTaskPtr = std::shared_ptr<std::function<void()>>;

struct REFL_API ITaskQueue
{
	virtual void PostTask(FTaskPtr TaskPtr) = 0;
	virtual uint32_t GetTaskNum() = 0;
	virtual uint32_t ProcessTask() = 0;
};

template<EQueueMode QueueMode>
struct FTaskQueue : public ITaskQueue
{
	virtual void PostTask(FTaskPtr TaskPtr) override
	{
		Queue.Enqueue(TaskPtr);
		TaskNum++;
	}

	virtual uint32_t ProcessTask() override
	{
		FTaskPtr TaskPtr;
		uint32_t Counter = 0;
		while (Queue.Dequeue(TaskPtr))
		{
			(*TaskPtr)();
			TaskNum--;
			Counter++;
		}
		return Counter;
	}

	virtual uint32_t GetTaskNum() override { return TaskNum; }

private:

	TQueue<FTaskPtr, QueueMode> Queue;
	std::atomic<uint32_t> TaskNum;
};

using FTaskSPSCQueue = FTaskQueue<EQueueMode::QM_SPSC>;
using FTaskMPSCQueue = FTaskQueue<EQueueMode::QM_MPSC>;

extern REFL_API std::unique_ptr<ITaskQueue> GMainThreadQueue;
extern REFL_API std::vector<std::unique_ptr<ITaskQueue>> GTaskThreadQueues;
//extern REFL_API std::vector<FTaskSPSCQueue> GTaskThreadQueues;

template<EThreadId ThreadId, typename ATask, typename ... AArgs>
void PostTask(ATask&& Task, AArgs&& ... Args)
{
	FTaskPtr TaskPtr =
		std::make_shared<FTask>(
			std::bind(
				std::forward<ATask>(Task),
				std::forward<AArgs>(Args)...));
	switch (ThreadId)
	{
	case TI_Main:
		GMainThreadQueue->PostTask(TaskPtr);
		break;
	default:
		if (TI_TaskMin <= ThreadId && ThreadId <= TI_TaskMax)
		{
			GTaskThreadQueues[ThreadId - TI_TaskMin]->PostTask(TaskPtr);
		}
		break;
	}
}

template<typename ATask, typename ... AArgs>
void PostTask(EThreadId ThreadId, ATask&& Task, AArgs&& ... Args)
{
	FTaskPtr TaskPtr = 
		std::make_shared<FTask>(
			std::bind(
				std::forward<ATask>(Task), 
				std::forward<AArgs>(Args)...));
	switch (ThreadId)
	{
	case TI_Main:
		GMainThreadQueue->PostTask(TaskPtr);
		break;
	default:
		if (TI_TaskMin <= ThreadId && ThreadId <= TI_TaskMax)
		{
			GTaskThreadQueues[ThreadId - TI_TaskMin]->PostTask(TaskPtr);
		}
		break;
	}
}


