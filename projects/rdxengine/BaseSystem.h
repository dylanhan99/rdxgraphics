#ifndef BASESYSTEM_H
#define BASESYSTEM_H
#include "BaseService.h"

#define RX_DECLARE_SYSTEM(name, ty)									   \
public:																   \
		inline const char* GetName() const override { return name; }   \
private:															   \
	friend class ServiceLayer;										   \
	inline static constexpr SystemType s_SystemType{ SystemType::ty };

namespace rdx
{
	enum class SystemType { // This declares ordering as well
		Window,
		Renderer,

		MAX
	};

	class BaseSystem : public BaseService
	{
	public:
		virtual ~BaseSystem() = default;

		bool Init();
		bool Terminate();
		void Update(float dt);
		void Draw();

	private:
		virtual bool InitImpl() = 0;
		virtual bool TerminateImpl() = 0;

		virtual void UpdateImpl(float dt) {};
		virtual void DrawImpl() {};
	};
}

#endif