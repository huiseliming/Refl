#include "AsyncTask.h"
#include "Logger.h"

std::unique_ptr<ITaskQueue> GMainThreadQueue = std::make_unique<FTaskMPSCQueue>();
std::vector<std::unique_ptr<ITaskQueue>> GTaskThreadQueues;

struct FTaskThread : public FTaskMPSCQueue
{
	FTaskThread(int32_t InIndex)
		: Index(InIndex)
		, bIsRequestExit(false)
		, Thread(&FTaskThread::Run, this)
	{}
	~FTaskThread()
	{
		bIsRequestExit = true;
		Thread.join();
	}
	void Run(){
		std::chrono::steady_clock::time_point TimePoint1, TimePoint2;
		TimePoint1 = std::chrono::steady_clock::now();
		while (true)
		{
			if (ProcessTask() != 0)
				TimePoint1 = std::chrono::steady_clock::now();
			if (IsRequestExit()) break;

			TimePoint2 = std::chrono::steady_clock::now();
			if (std::chrono::duration<double>(TimePoint2 - TimePoint1).count() > 3.0f)
				std::this_thread::sleep_for(std::chrono::milliseconds(33));
			else
				std::this_thread::yield();
		}
		if (GetTaskNum() != 0)
			GLOG(Warning, "TASK THREAD {:d} HAS {:d} UNPROCESSED TASK", Index, GetTaskNum());
	}
	bool IsRequestExit() { return bIsRequestExit; }

protected:
	int32_t Index;
	std::atomic<bool> bIsRequestExit;
	std::thread Thread;
};

ITaskQueue* GetThreadTaskQueue(EThreadId ThreadId)
{
	switch (ThreadId)
	{
	case TI_Main:
		return GMainThreadQueue.get();
		break;
	default:
		if (TI_TaskMin <= ThreadId && ThreadId <= TI_TaskMax)
		{
			int32_t Index = ThreadId - TI_TaskMin;
			if (Index < GTaskThreadQueues.size())
			{
				return GTaskThreadQueues[Index].get();
			}
			while (Index >= GTaskThreadQueues.size())
			{
				int32_t Index = GTaskThreadQueues.size();
				GTaskThreadQueues.emplace_back(std::make_unique<FTaskThread>(Index));
			}
			return GTaskThreadQueues[Index].get();
		}
		break;
	}
	return nullptr;
}
