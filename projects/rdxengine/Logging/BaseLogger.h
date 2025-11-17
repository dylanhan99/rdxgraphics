#ifndef BASELOGGER_H
#define BASELOGGER_H
#include "RXAPI.h"
#include "BaseService.h"

#define RX_TRACE(fmt, ...)	  ServiceLayer::LoggingService()->Log("[trace] [{}] - \n"    fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_DEBUG(fmt, ...)	  ServiceLayer::LoggingService()->Log("[debug] [{}] - \n"    fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_INFO(fmt, ...)	  ServiceLayer::LoggingService()->Log("[info] [{}] - \n"	 fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_WARN(fmt, ...)	  ServiceLayer::LoggingService()->Log("[warn] [{}] - \n"	 fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_ERROR(fmt, ...)	  ServiceLayer::LoggingService()->Log("[error] [{}] - \n"    fmt, __FUNCTION__, ##__VA_ARGS__)
#define RX_CRITICAL(fmt, ...) ServiceLayer::LoggingService()->Log("[critical] [{}] - \n" fmt, __FUNCTION__, ##__VA_ARGS__)

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

	class RX_API BaseLogger : public BaseService
	{
	public:
		virtual ~BaseLogger();
		//virtual bool Init() = 0; // These are pure virtual from BaseService already
		//virtual bool Terminate() = 0;

		virtual void HandleEntryImpl(LogEntry const&) = 0;

		inline void HandleEntry(LogEntry const& entry)
		{
			if (IsInitialized())
				HandleEntryImpl(entry);
			else
				std::cerr << "@@@ Logger uninitialized. Last log attempt:\n" << entry.FnFormatter() << std::endl;
		}

		template <typename ...Args>
		void Log(std::string format, Args&&... args)
		{
			LogEntry entry{};
			entry.FnFormatter =
				[fmt = std::move(format), ...capturedArgs = std::forward<Args>(args)]()
				{
					return std::vformat(fmt, std::make_format_args(capturedArgs...));
				};

			HandleEntry(std::move(entry));
		}
	};
}

#endif