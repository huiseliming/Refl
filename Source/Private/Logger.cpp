#include "Logger.h"
#include <cassert>
#include <iostream>
#include <format>
#include "AsyncTask.h"

FLogger* GLog = GLogInitializer();

std::string FormatSystemTime(const time_t& tt)
{
	tm* NowTmPtr = localtime(&tt);
	return std::format("{:d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}",
		NowTmPtr->tm_year + 1900, NowTmPtr->tm_mon + 1, NowTmPtr->tm_mday,
		NowTmPtr->tm_hour, NowTmPtr->tm_min, NowTmPtr->tm_sec);
}
std::string GetCurrentSystemTime()
{
	return FormatSystemTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
}

FLogger::FLogger()
	: bRequestStop(false)
	, Thread(&FLogger::Run, this)
{
}

FLogger::FLogger(std::shared_ptr<std::function<void(const FLogMessage&)>> LogHandler)
	: LogHandlers({ LogHandler })
	, bRequestStop(false)
	, Thread(&FLogger::Run, this)
{
	assert(LogHandler);
}

FLogger::~FLogger()
{
	bRequestStop = true;
	Thread.join();
}

void FLogger::Log(ELogVerbosityLevel LogVerbosity, const std::string& Message)
{
	//assert(Running == true);
	LogMessageQueue.Enqueue(FLogMessage{
		std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()),
		std::this_thread::get_id(),
		LogVerbosity,
		Message.data()
		});
}

void FLogger::Log(ELogVerbosityLevel LogVerbosity, std::string&& Message)
{
	//assert(Running == true);
	LogMessageQueue.Enqueue(FLogMessage{
		std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()),
		std::this_thread::get_id(),
		LogVerbosity,
		std::move(Message)
		});
}

void FLogger::Run()
{
	time_t Now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
#ifdef _WIN32
	tm NowTm = { 0 };
	tm* NowTmPtr = &NowTm;
	localtime_s(NowTmPtr, &Now);
#else
	tm* NowTmPtr = localtime(&Now);
#endif // DEBUG
	while (!bRequestStop) {
		Loop();
	}
	Loop();
}

void FLogger::Loop() {
	std::function<void()> Task;
	while (TaskQueue.Dequeue(Task))
	{
		Task();
	}
	FLogMessage LogMessage;
	while (LogMessageQueue.Dequeue(LogMessage))
	{
		for (size_t i = 0; i < LogHandlers.size(); i++)
		{
			(*LogHandlers[i])(LogMessage);
		}
	}
	std::this_thread::yield();
}

const char* ToString(ELogVerbosityLevel LogVerbosity)
{
	switch (LogVerbosity)
	{
	case ELogVerbosityLevel::LVL_Trace:
		return "Trace";
	case ELogVerbosityLevel::LVL_Debug:
		return "Debug";
	case ELogVerbosityLevel::LVL_Info:
		return "Info";
	case ELogVerbosityLevel::LVL_Warning:
		return "Warning";
	case ELogVerbosityLevel::LVL_Error:
		return "Error";
	case ELogVerbosityLevel::LVL_Fatal:
		return "Fatal";
	default:
		break;
	}
	return "None";
}

FLogger* GLogInitializer()
{
	static std::function<FLogger* ()> Initializer = []() {
		static FLogger Logger(
			std::make_shared<std::function<void(const FLogMessage&)>>(
				[](const FLogMessage& LogMessage) {
					std::stringstream StringStream;
					StringStream << LogMessage.ThreadId;
					std::string FormatLog =
						std::format("[{:s}] [{:<7s}] [{:<7s}] {:s}",
							FormatSystemTime(LogMessage.Timestamp),
							StringStream.str(),
							ToString(LogMessage.LogLevel),
							LogMessage.Message);
					if (FormatLog.back() != '\n')
					{
						FormatLog.push_back('\n');
					}
					std::cout << FormatLog;
				}));
		return &Logger;
	};
	static FLogger* LoggerPtr = Initializer();
	return LoggerPtr;
}
