#pragma once

// See https://docs.unity3d.com/460/Documentation/Manual/ExecutionOrder.html for phases
enum class EventPhase
{
    Reset,
    StartFrame,
    FixedUpdate,
    OnInput,
    Update,
    LateUpdate,
    OnPreRender,
    OnRender,
    OnPostRender,
    OnGUI,
    EndFrame,
    ApplicationQuit,

    MAX
};

// GLFW events
struct FrameBufferResizeEvent { int x, y; };
struct WindowPositionEvent { int x, y; };
struct WindowResizeEvent { int x, y; };

// Input events
struct RawKeyInputEvent { int key, scancode, action; };
struct RawButtonInputEvent { int button, action; };
struct RawCursorMovedEvent { double xpos, ypos; };
struct RawScrollEvent { double xoffset, yoffset; };

struct KeyInputEvent { int key; InputAction action; };
struct MouseInputEvent { int button; double offset; InputAction action; };
struct CursorMovedEvent { glm::vec2 position; };

// Graphics events
struct ToggleCameraEvent { class Camera* pCamera; };
