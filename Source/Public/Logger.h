#pragma once
#include "CppStandardLibrary.h"
#include "ReflExport.h"
#include "Queue.h"


enum class ELogVerbosityLevel
{
	LVL_Trace,
	LVL_Debug,
	LVL_Info,
	LVL_Warning,
	LVL_Error,
	LVL_Fatal,
};

struct FLogMessage
{
	FLogMessage() = default;
	FLogMessage(time_t Timestamp, std::thread::id ThreadId, ELogVerbosityLevel LogLevel, const std::string& Message)
		: Timestamp(Timestamp)
		, ThreadId(ThreadId)
		, LogLevel(LogLevel)
		, Message(Message)
	{
	}
	FLogMessage(time_t Timestamp, std::thread::id ThreadId, ELogVerbosityLevel LogLevel, std::string&& Message)
		: Timestamp(Timestamp)
		, ThreadId(ThreadId)
		, LogLevel(LogLevel)
		, Message(std::move(Message))
	{
	}

	FLogMessage(const FLogMessage&) = default;
	FLogMessage(FLogMessage&& Other)
	{
		Timestamp = Other.Timestamp;
		ThreadId = Other.ThreadId;
		LogLevel = Other.LogLevel;
		Other.Timestamp = { 0 };
		Other.ThreadId = std::thread::id();
		Other.LogLevel = ELogVerbosityLevel::LVL_Trace;
		Message = std::move(Other.Message);
	}

	FLogMessage& operator=(const FLogMessage&) = default;
	FLogMessage& operator=(FLogMessage&& Other)
	{
		assert(std::addressof(Other) != this);
		Timestamp = Other.Timestamp;
		ThreadId = Other.ThreadId;
		LogLevel = Other.LogLevel;
		Other.Timestamp = { 0 };
		Other.ThreadId = std::thread::id();
		Other.LogLevel = ELogVerbosityLevel::LVL_Trace;
		Message = std::move(Other.Message);
		return *this;
	}

	time_t Timestamp;
	std::thread::id ThreadId;
	ELogVerbosityLevel LogLevel;
	std::string Message;
};

namespace std {
	template<>
	struct hash<FLogMessage> {
		using result_type = size_t;
		using argument_type = FLogMessage;
		uint64_t operator()(const FLogMessage& InLogMessage) const {
			return std::hash<std::thread::id>()(InLogMessage.ThreadId) ^
				std::size_t(InLogMessage.LogLevel) ^
				std::hash<std::string>()    (InLogMessage.Message);
		}
	};

	template<>
	struct equal_to<FLogMessage> {
		typedef FLogMessage first_argument_type;
		typedef FLogMessage second_argument_type;
		typedef bool result_type;

		uint64_t operator()(const FLogMessage& RightLogMessage, const FLogMessage& LeftLogMessage) const {
			return RightLogMessage.ThreadId == LeftLogMessage.ThreadId &&
				RightLogMessage.LogLevel == LeftLogMessage.LogLevel &&
				RightLogMessage.Message == LeftLogMessage.Message;
		}
	};
}

class REFL_API FLogger
{
public:
	FLogger();
	FLogger(std::shared_ptr<std::function<void(const FLogMessage&)>> DefaultLogHandler);

	~FLogger();

	void Log(ELogVerbosityLevel LogVerbosity, const std::string& Message);

	void Log(ELogVerbosityLevel LogVerbosity, std::string&& Message);

	template<typename ... TArgs>
	void Log(ELogVerbosityLevel LogVerbosity, const std::string_view Message, TArgs&& ... Args)
	{
		Log(LogVerbosity, std::format(Message, std::forward<TArgs>(Args)...));
	}

	std::future<void> AddLogHandlers(std::shared_ptr<std::function<void(const FLogMessage&)>> LogHandler)
	{
		std::shared_ptr<std::promise<void>> Promise = std::make_shared<std::promise<void>>();
		std::future<void> Future = Promise->get_future();
		TaskQueue.Enqueue([&, LogHandler, Promise] {
				LogHandlers.push_back(LogHandler);
				Promise->set_value();
			});
		return Future;
	}

	std::future<bool> RemoveLogHandlers(std::shared_ptr<std::function<void(const FLogMessage&)>> LogHandler)
	{
		std::shared_ptr<std::promise<bool>> Promise = std::make_shared<std::promise<bool>>();
		std::future<bool> Future = Promise->get_future();
		TaskQueue.Enqueue([&, LogHandler, Promise]() {
			for (auto it = LogHandlers.begin(); it != LogHandlers.end(); it++)
			{
				if (*it == LogHandler) {
					LogHandlers.erase(it);
					Promise->set_value(true);
					return;
				}
			}
			Promise->set_value(false);
			});
		return Future;
	}

	//void Log(ELogLevel LogLevel, const std::string_view Message);
	//template<typename ... TArgs>
	//void Log(ELogLevel LogLevel, std::string&& Message, TArgs&& ... Args)
	//{
	//	Log(LogLevel, std::format(std::forward<std::string>(Message), std::forward<TArgs>(Args)...));
	//}

protected:
	void Run();

	void Loop();

	std::vector<std::shared_ptr<std::function<void(const FLogMessage& LogMessage)>>> LogHandlers;
	std::atomic<bool> bRequestStop;
	std::thread Thread;

	TQueue<std::function<void()>, EQueueMode::QM_MPSC> TaskQueue;
	TQueue<FLogMessage, EQueueMode::QM_MPSC> LogMessageQueue;
};

REFL_API const char* ToString(ELogVerbosityLevel LogLevel);
REFL_API FLogger* GLogInitializer();

extern REFL_API FLogger* GLog;
#define GLOG(Verbosity, FMT, ...) GLog->Log(ELogVerbosityLevel::LVL_##Verbosity, FMT, ##__VA_ARGS__)










struct CLogScopeTimeConsume
{

};

