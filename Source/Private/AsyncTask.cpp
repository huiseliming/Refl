#include "AsyncTask.h"

std::unique_ptr<ITaskQueue> GMainThreadQueue = std::make_unique<FTaskMPSCQueue>();
std::vector<std::unique_ptr<ITaskQueue>> GTaskThreadQueues;

