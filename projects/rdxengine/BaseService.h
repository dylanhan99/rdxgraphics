#ifndef BASESERVICE_H
#define BASESERVICE_H

namespace rdx
{
	class BaseService
	{
	public:
		virtual ~BaseService() = default;

		inline bool IsInitialized() const { return m_IsInitialized; }
		inline virtual std::string GetName() const { return "Missing Name"; }

	protected:
		bool m_IsInitialized{ false };
	};
}

#endif