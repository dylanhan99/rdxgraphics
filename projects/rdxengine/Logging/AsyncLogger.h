#ifndef ASYNCLOGGER_H
#define ASYNCLOGGER_H
#include "RXAPI.h"

namespace rdx
{
	struct LogMessage
	{
		std::string message{};
	};

	class RX_API AsyncLogger
	{
	public:
		bool Init();
		bool Terminate();

		void Log(LogMessage const& msg);
		bool ShouldStop() const;

	private:
		void WorkerLoop();
		void WriteLog(LogMessage const& msg);

	private:
		std::thread m_WorkerThread{};
		std::condition_variable m_CV{};
		std::mutex m_Mutex{};

		std::queue<LogMessage> m_Queue{};
		bool m_ShouldStop{ false };
	};
}

#endif