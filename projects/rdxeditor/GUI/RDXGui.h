#ifndef RDXGUI_H
#define RDXGUI_H
#include "rdxengine/BaseApp.h"
#include "BasePanel.h"

namespace rdxgui
{
	class RDXGui : public rdx::BaseApp
	{
	public:
		inline bool IsEnabled() const { return m_IsEnabled; }

	private:
		bool Init() override;
		bool Terminate() override;

		void FrameStartImpl() override;
		void FrameEndImpl() override;

		template <typename T>
		void RegisterPanel(const char* windowName, ImGuiWindowFlags flags = 0)
		{
			m_Panels.emplace_back(std::make_unique<T>(windowName, flags));
		}

	private:
		bool m_IsEnabled{ false };

		std::vector<std::unique_ptr<BasePanel>> m_Panels{};
	};
}

#endif