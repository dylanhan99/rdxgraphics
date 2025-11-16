#ifndef ASYNCLOGGER_H
#define ASYNCLOGGER_H
#include "RXAPI.h"

#define RX_TRACE(fmt, ...)	  ServiceLayer::LoggingService()->Log("[trace] {} - \n"    fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_DEBUG(fmt, ...)	  ServiceLayer::LoggingService()->Log("[debug] {} - \n"    fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_INFO(fmt, ...)	  ServiceLayer::LoggingService()->Log("[info] {} - \n"	   fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_WARN(fmt, ...)	  ServiceLayer::LoggingService()->Log("[warn] {} - \n"	   fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_ERROR(fmt, ...)	  ServiceLayer::LoggingService()->Log("[error] {} - \n"    fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_CRITICAL(fmt, ...) ServiceLayer::LoggingService()->Log("[critical] {} - \n" fmt, __FUNCTION__, ##__VA_ARGS__)

namespace rdx
{
	enum class LogLevel : char
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical
	};

	struct LogEntry
	{
		std::function<std::string()> FnFormatter{}; // Lambda capturing all entries. Type erased storage.
	};

	class RX_API AsyncLogger
	{
	public:
		bool Init();
		bool Terminate();

		template <typename ...Args>
		void Log(std::string format, Args&&... args)
		{
			LogEntry entry{};
			entry.FnFormatter =
				[fmt = std::move(format), ...capturedArgs = std::forward<Args>(args)]()
				{
					return std::vformat(fmt, std::make_format_args(capturedArgs...));
				};

			EnqueueEntry(std::move(entry));
		}

		void EnqueueEntry(LogEntry const&);
		bool ShouldStop() const;

	private:
		void WorkerLoop();
		void WriteLog(LogEntry const&);

	private:
		std::thread m_WorkerThread{};
		std::condition_variable m_CV{};
		std::mutex m_Mutex{};

		std::queue<LogEntry> m_Queue{};
		bool m_ShouldStop{ false };
	};
}

#endif