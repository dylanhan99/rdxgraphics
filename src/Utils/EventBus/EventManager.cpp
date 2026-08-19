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
	/*** OnInput ***/
	// Handle Input system first
	EventBus::Subscribe(EventPhase::OnInput, static_cast<(*)(const RawKeyInputEvent&)>Input::OnInputEvent);
	EventBus::Subscribe(EventPhase::OnInput, static_cast<(*)(const RawButtonInputEvent&)>Input::OnInputEvent);
	EventBus::Subscribe(EventPhase::OnInput, static_cast<(*)(const RawCursorMovedEvent&)>Input::OnInputEvent);
	EventBus::Subscribe(EventPhase::OnInput, static_cast<(*)(const RawScrollEvent&)>Input::OnInputEvent);

	// Then other systems like Imgui
	EventBus::Subscribe(EventPhase::OnInput, static_cast<(*)(const KeyInputEvent&)>Input::OnInputEvent);
	EventBus::Subscribe(EventPhase::OnInput, static_cast<(*)(const MouseInputEvent&)>Input::OnInputEvent);
	EventBus::Subscribe(EventPhase::OnInput, static_cast<(*)(const CursorMovedEvent&)>Input::OnInputEvent);
}