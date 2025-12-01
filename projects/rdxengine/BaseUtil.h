#ifndef BASEUTIL_H
#define BASEUTIL_H
#include "BaseService.h"

#define RX_DECLARE_UTIL(name, ty)									   \
public:																   \
		inline std::string GetName() const override { return name; }   \
private:															   \
	friend class ServiceLayer;										   \
	inline static constexpr UtilType s_UtilType{ UtilType::ty };

namespace rdx
{
	enum class UtilType { // Probably don't need this
		Input,
		Logger,
		ECS,
		InstantEvent,
		PerformanceProfiler,

		MAX
	};

	class BaseUtil : public BaseService
	{
	public:
		virtual ~BaseUtil() = default;

		bool Init();
		bool Terminate();

	private:
		virtual bool InitImpl() { return true; };
		virtual bool TerminateImpl() { return true; };
	};
}

#endif