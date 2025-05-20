# CS350/CSD3150 | Project 1: Geometry Toolbox
By: 2201020 | dylan.h | Han Wei, Dylan
Engine: rdxgraphics

## Project Context
### Environment/Tech
- CMake
- git
- Platform Visual Studio 2022 v143 (MSVC)
- C++20
- OpenGL/GLEW/GLFW/glm/ImGUI/spdlog/EnTT

### Machines Tested On
- Laptop 1, Windows 11, Intel i7-11800H, NVIDIA GeForce RTX 3060 (Laptop), OpenGL 4.5
- Laptop 2, Windows 10, Intel i7-7500U,  NVIDIA GeForce 940MX			 , OpenGL 4.5
- Lab PC (Multiple), did not take note of specs but all ran fine 		 , OpenGL 4.5

### CMake Changes
Minor edits made to the cmake files provided by the professor.
- Removed the include/ directory in the project to streamline my own workflow. This includes editing the cmake file to only GLOB_RECURSE over src/ for .h/.hpp/.cpp.
- Added /Zc:preprocessor in MSVC target_compile_options. This allows me to use __VA_OPT__ C++20 preprocessor feature.
- Added setup for other libraries as instructed in CMaksLists.txt and ImportDependencies.cmake. Libraries added include glm, ImGUI, spdlog and EnTT.

### Project Structure
Main (/):
entrypoint.cpp (main) calls the entry to the application declared in RDX.h/.cpp.
RDX.h/.cpp contains the application-loop.

GLFW (GLFWWindow/):
- Wrapper class around GLFW window functionality. Does the standard initialization, as well as event setups.
- This is the only file which includes glfw headers

ECS (ECS/):
- The project is very much ECS based, so each system (ECS/Systems/*) only touches the components relevant to them.
- Components (ECS/Components/*) are simple datastructures typically only with getter/setter funcitons. Data/lifetime managed by systems.
- EntityManager.h/cpp wraps around EnTT interface.

Graphics (Graphics/):
- Wrapper classes around VAO/VBO (Object.h), FBO (RenderPass.h), Shaders (Shader.h), UBO (UniformBuffer.h)
- Usage can mainly be seen in ECS/Systems/RenderSystem.*
- This engine does multi-pass rendering, it requires one set of "main" shaders for our standard rendering, and another set of "screen" shaders to combine the results of each pass into the output we see.

GSM/GUI (GSM/, GUI/):
- Wrappers around "scripting" and ImGUI windows respectively.
- GSM manages a collection of registered scenes. Each scene provides behaviour for a specific collision test (in the case of this project).
- The ImGUI wrappers provide a way to isolate each window of the "editor" to make things neater.

Utils (Utils/):
- Utility tools such as input, logging, Singleton, event dispatcher

## How to Use
### Key Binds
ESC (Trigger)	- Kill application
TAB (Trigger)	- Toggles FPS camera control
W/A/S/D (Hold)	- Move FPS camera forward/backward/left/right (While camera toggled)
Move mouse		- Pitch/Yaw FPS camera (While camera toggled)
F11 (Trigger)	- Toggle window enlarge/shrink

### GUI Guide
Entity Hierarchy:
- Lists entities within each active scene.
- You may select an entity, which allows you to inspect its components.
Inspector:
- Displays the selected entity's components, and the attributes (only Xform, Camera, Directional light, and Collider/BV are supported now)
- Collider/BV's position/xform is separate from Xform component's position/xform. But I made it sync through the TransformaSystem for convenience for now. This can be toggled on/off through the Collider/BV component.
- You may change the Collider's BV on the fly without recompilation.
- There is no [Add Component] function (not yet).
Viewport:
- Displays two camera views by default, FPS (Perspective) and PiP (Ortho).
- Likely need to fullscreen the app to be able to see the PiP on the top right.

## Tasks
Task 1: Window Class								> Complete
Task 2/3: Buffer class VBO and Attribute management > Complete
Task 4: ECS											> Complete
Task 5: Light										> Complete
Task 6: Interactivity (Camera)						> Complete
Collision Tests										> All complete

### Task Code Locations
Task 1:	  GLFWWindow/GLFWWindow.\*, used in RDX.cpp
Task 2/3: Graphics/\*, used mainly in ECS/Systems/RenderSystem.cpp
Task 4:	  ECS/\*, used mainly in ECS/Systems/\*
Task 5: 
- ECS/Components/BaseComponent.h, class DirectionalLight
- ECS/Systems/RenderSystem.cpp, submit light component data to shader
- shaders/default.frag			
Task 6:
- ECS/Systems/CollisionSystem.cpp, the full suite (as stated by brief) of CheckCollision(FooBV&, GooBV&) overloads.
- ECS/Components/Colliders.\*, BV components declared here

## Other Notes
- I copy pasted the keymappings from glfw.h into Input.h to mask the same mappings with my own alias instead (RX_ prefix). This is pretty bad.)
- Average 4 hours daily on this assignment