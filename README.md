# CS350/CSD3150 | Project 1: Geometry Toolbox
By: 2201020 | dylan.h | Han Wei, Dylan
Engine: rdxgraphics

## Project Context
### CMake Changes
Minor edits made to the cmake files provided by the professor.
- Removed the include/ directory in the project to streamline my own workflow. This includes editing the cmake file to only GLOB_RECURSE over src/ for .h/.hpp/.cpp.
- Added /Zc:preprocessor in MSVC target_compile_options. This allows me to use __VA_OPT__ C++20 preprocessor feature.
- Added setup for other libraries as instructed, in CMaksLists.txt and ImportDependencies.cmake. Libraries include glm, ImGUI, spdlog and EnTT.
### Project Structure
Main (/):
entrypoint.cpp (main) calls the entry to the application declared in RDX.h/.cpp.
RDX.h/.cpp contains the application-loop.
It looks like I use precompiled headers in this project, but I don't think the CMake settings actually reflect that. So the project "uses precompiled headers".

GLFW (GLFWWindow/):
- Wrapper class around GLFW window functionality. Does the standard initialization, as well as event setups.
- This is the only file which includes glfw headers

ECS (ECS/):
- The project is very much ECS based, so each system (ECS/Systems/*) only touches the components relevant to them.
- Components (ECS/Components/*) are simple datastructures typically only with getter/setter funcitons. Data/lifetime managed by systems.
- EntityManager.h/cpp

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

## Tasks
Task 1: Window Class > Complete
Task 2/3: Buffer class VBO and Attribute management > Complete
Task 4: ECS > Complete
Task 5: Light > Complete
Task 6: Interactivity (Camera) > Complete
Collision Tests > All complete
### Task Code Locations
Task 1: GLFWWindow/GLFWWindow.*, used in RDX.cpp
Task 2/3: Graphics/*, used mainly in ECS/Systems/RenderSystem.cpp
Task 4: ECS/*, used mainly in ECS/Systems/\*
Task 5: 
- ECS/Components/BaseComponent.h, class DirectionalLight
- ECS/Systems/RenderSystem.cpp, submit light component data to shader
- shaders/default.frag
Task 6: ECS/Systems/CollisionSystem.cpp, the full suite (as stated by brief) of CheckCollision(FooBV&, GooBV&) overloads.