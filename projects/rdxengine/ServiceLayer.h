#ifndef RDXENGINE_H
#define RDXENGINE_H
#include "Input/Input.h"

namespace rdx
{
	class ServiceLayer
	{
		friend class RDX;
	public:
		static ServiceLayer* const Get();
		static Input* const InputService();

	private:
		void Init();
		void Terminate();

	private:
		inline static ServiceLayer* m_ServiceLayer{};
		Input* m_InputService{};
	};
}

#endif