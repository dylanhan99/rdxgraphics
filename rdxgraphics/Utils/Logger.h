#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define _RX_LOG_WRAP(level, fmt, ...) {std::lock_guard<std::mutex> lg{Logger::GetMutex()}; Logger::GetLogger().level(fmt, ##__VA_ARGS__);}
#define RX_TRACE(fmt, ...)	  _RX_LOG_WRAP(trace,	 "{} - " fmt, __func__, ##__VA_ARGS__)
#define RX_DEBUG(fmt, ...)	  _RX_LOG_WRAP(debug,	 "{} - " fmt, __func__, ##__VA_ARGS__)
#define RX_INFO(fmt, ...)	  _RX_LOG_WRAP(info,	 "{} - " fmt, __func__, ##__VA_ARGS__)
#define RX_WARN(fmt, ...)	  _RX_LOG_WRAP(warn,	 "{} - " fmt, __func__, ##__VA_ARGS__)
#define RX_ERROR(fmt, ...)	  _RX_LOG_WRAP(error,	 "{} - " fmt, __func__, ##__VA_ARGS__)
#define RX_CRITICAL(fmt, ...) _RX_LOG_WRAP(critical, "{} - " fmt, __func__, ##__VA_ARGS__)

#define RX_EXCEPTION(msg) std::exception{msg}
#ifdef _DEBUG
#define RX_ASSERT(x, ...) if(!(x)){__VA_OPT__(RX_CRITICAL(__VA_ARGS__);) assert((#x , false));}
#else
#define RX_ASSERT(x, ...)
#endif

class Logger : public BaseSingleton<Logger>
{
	RX_SINGLETON_DECLARATION(Logger);
public:
	inline static void Init()
	{
		spdlog::init_thread_pool(2048, 2);
		std::vector<spdlog::sink_ptr> sinks{
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		};

		g.m_Logger.reset(
			new spdlog::async_logger(
				"FF_AsyncLogger",
				sinks.begin(), sinks.end(), spdlog::thread_pool()
			));

		g.m_Logger->set_level(spdlog::level::trace);
		spdlog::register_logger(g.m_Logger);
	}

	inline static void Terminate()
	{
		spdlog::shutdown();
	}

	inline static spdlog::logger& GetLogger() { return *g.m_Logger; }
	inline static std::mutex& GetMutex() { return g.m_Mutex; }

private:
	std::shared_ptr<spdlog::logger> m_Logger{};
	std::mutex m_Mutex{};
};

//

template <>
struct fmt::formatter<std::filesystem::path> : fmt::formatter<std::string>
{
	template <typename FormatContext>
	auto format(std::filesystem::path const& p, FormatContext& ctx) const
	{
		return fmt::formatter<std::string> ::format(p.generic_string(), ctx);
	}
};