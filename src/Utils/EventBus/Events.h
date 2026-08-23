#pragma once

// See https://docs.unity3d.com/460/Documentation/Manual/ExecutionOrder.html for phases
enum class EventPhase
{
    OnReset,
    OnStartFrame,
    OnFixedUpdate,
    OnInput,
    OnUpdate,
    OnLateUpdate,
    OnPreRender,
    OnRender,
    OnPostRender,
    OnGUI,
    OnEndFrame,
    OnApplicationQuit,

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

// Graphics events
struct ToggleCameraEvent { class Camera* pCamera; };
