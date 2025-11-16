#ifndef ASYNCLOGGER_H
#define ASYNCLOGGER_H
#include "BaseLogger.h"

namespace rdx
{
	class AsyncLogger : public BaseLogger
	{
	public:
		inline ~AsyncLogger() {};
		bool Init() override;
		bool Terminate() override;

		void HandleEntry(LogEntry const&) override;
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