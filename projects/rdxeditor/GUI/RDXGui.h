#ifndef RDXGUI_H
#define RDXGUI_H
#include "rdxengine/BaseApp.h"
#include "BasePanel.h"
#include "ECS/Components/Component.h"
#include "ECS/Components/CameraComponent.h"

namespace rdxgui
{
	class RDXGui : public rdx::BaseApp
	{
	public:
		inline bool IsEnabled() const { return m_IsEnabled; }
		inline rdx::TransformComponent& GetCameraPosition() { return m_EditorCameraPos; }
		inline rdx::CameraComponent& GetCamera() { return m_EditorCamera; }

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

		void MenuBar();

	private:
		bool m_IsEnabled{ false };

		rdx::TransformComponent m_EditorCameraPos{};
		rdx::CameraComponent m_EditorCamera{};
		rdx::CameraComponent m_GameCamera{}; // Editor will own this for now, while there still is no entity with camera available.

		std::vector<std::unique_ptr<BasePanel>> m_Panels{};
	};
}

#endif