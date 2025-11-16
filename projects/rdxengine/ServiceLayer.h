#ifndef RDXENGINE_H
#define RDXENGINE_H
#include "Window/BaseWindow.h"
#include "Input/Input.h"

namespace rdx
{
	class ServiceLayer
	{
		friend class RDX;
	public:
		static ServiceLayer* const Get();
		static BaseWindow* const WindowService();
		static Input* const InputService();

	private:
		bool Init();
		bool Terminate();

	private:
		inline static ServiceLayer* m_ServiceLayer{};
		BaseWindow* m_WindowService{};
		Input* m_InputService{};
	};
}

#endif