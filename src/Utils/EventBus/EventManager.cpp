#include "EventManager.h"
#include "EventBus.h"

#include "ECS/Components.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/TransformSystem.h"
#include "ECS/Systems/BVHSystem.h"
#include "GLFWWindow/GLFWWindow.h"
#include "GSM/SceneManager.h"
#include "GSM/Scenes/CommonLayer.h"
#include "GSM/Scenes/Sandbox.h"
#include "GUI/GUI.h"
#include "Utils/Input.h"


void EventManager::Init()
{
	/*** StartFrame ***/
	// Process OS Input
	EventBus::Subscribe(EventPhase::StartFrame, GLFWWindow::StartFrame, -999); // Must be first
	EventBus::Subscribe(EventPhase::StartFrame, static_cast<void(*)(const RawKeyInputEvent&)>(Input::OnInputEvent));
	EventBus::Subscribe(EventPhase::StartFrame, static_cast<void(*)(const RawButtonInputEvent&)>(Input::OnInputEvent));
	EventBus::Subscribe(EventPhase::StartFrame, static_cast<void(*)(const RawCursorMovedEvent&)>(Input::OnInputEvent));
	EventBus::Subscribe(EventPhase::StartFrame, static_cast<void(*)(const RawScrollEvent&)>(Input::OnInputEvent));

	/*** OnInput ***/
	// Editor GUI
	EventBus::Subscribe(EventPhase::OnInput, static_cast<void(*)(const KeyInputEvent&)>(GUI::OnInputEvent));
	EventBus::Subscribe(EventPhase::OnInput, static_cast<void(*)(const MouseInputEvent&)>(GUI::OnInputEvent));
	EventBus::Subscribe(EventPhase::OnInput, static_cast<void(*)(const CursorMovedEvent&)>(GUI::OnInputEvent));

	/*** EndFrame ***/
	EventBus::Subscribe(EventPhase::EndFrame, GLFWWindow::EndFrame, +999); // Must be last
}