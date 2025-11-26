#ifndef BASESERVICE_H
#define BASESERVICE_H

namespace rdx
{
	class BaseService
	{
	public:
		virtual ~BaseService() {};

		inline bool Init()
		{
			if (IsInitialized())
				return false;

			return m_IsInitialized = InitImpl();
		}

		inline bool Terminate()
		{
			bool success = false;
			if (IsInitialized())
				success = TerminateImpl();

			m_IsInitialized = false;
			return success;
		}

		inline bool IsInitialized() const { return m_IsInitialized; }

	private:
		virtual bool InitImpl() = 0;
		virtual bool TerminateImpl() = 0;

	private:
		bool m_IsInitialized{ false };
	};
}

#endif