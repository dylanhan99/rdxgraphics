#include "AsyncLogger.h"

using namespace rdx;

bool AsyncLogger::Init()
{
	m_WorkerThread = std::thread{ &AsyncLogger::WorkerLoop, this };
	return true;
}

bool AsyncLogger::Terminate()
{
	{
		std::lock_guard lock{ m_Mutex };
		m_ShouldStop = true;
	}

	m_CV.notify_one();
	m_WorkerThread.join();
	return true;
}

void rdx::AsyncLogger::Log(LogMessage const& msg)
{
	{
		std::lock_guard lock{ m_Mutex };
		m_Queue.push(msg);
	}
	m_CV.notify_one();
}

bool rdx::AsyncLogger::ShouldStop() const
{
	return m_ShouldStop;
}

void AsyncLogger::WorkerLoop()
{
	while (true)
	{
		std::unique_lock lock{ m_Mutex };

		// Sleep until message queue is empty, or it's time to stop the thread.
		m_CV.wait(lock,
			[this]()
			{
				return !m_Queue.empty() || ShouldStop();
			});

		// Queue must be completely flushed before exiting
		if (ShouldStop() && m_Queue.empty())
			break;

		LogMessage msg = m_Queue.front();
		m_Queue.pop();
		lock.unlock(); // Explicitly release lock

		// Then finally do the slow stuff, while the queue is unlocked
		WriteLog(msg);
	}
}

void rdx::AsyncLogger::WriteLog(LogMessage const& msg)
{
	std::cout << msg.message << std::endl;
}
