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

### How to use the Application
Prenote: 
- The use of the postfix "BV" is somewhat wrongly used. Think of it as "Collider" instead.
- [TAB] toggles the FPS camera control
The ImGUI window names are refered to as they are in the app. ("Hierarchy", "Viewport", "Inspector", "Settings")
Within the Hierarchy, there are two scenes, "Common" and "Sandbox".
Common includes 3 entities, those being - FPS (Perspective) camera, PiP (Ortho) camera, and the light.
Sandbox includes 2 entities, those being - Collider A, and Collider B.
> 

First some context on how to use the GUI, you may first select the entity (in Hierarchy), then in Inspector, "Choose BV Type" from the combo box.
Whichever BV you select, the attributes can be seen under the combo box, as a dropdown tree node.
It should be noted that the BVs have a separate positional attribute from the xform component. However, the "Follow Xform" tag is enabled by default to follow the xform. (For triangle related tests, this should be FALSE for the triangle to allow position(triangle centroid) to move).

So now that you've changed the pair of BVs to whichever test you're trying to perform, say Triangle-Ray, you may change the translation of the entities using "Pos" in Xform component (As mentioned before, for triangle, disable "FollowXform" in the BV attributes then change position there). 
As for the BV attributes, you change them in Collider component, under the combobox as mentioned before.

Notes:
- BVs with "orientation" or "normal" attributes such as PlaneBV or RayBV have **euler-based** orientations, Pitch(x)-Yaw(y)-Roll(z), expressed in Radians.
- TriangleBV must uncheck "Follow Xform", and move position from there. NOT from Xform component.
- TriangleBV has P0, P1, P2, which when changed, updates the Collider's position (centroid), hence the above note.
- TriangleBV's 3 points are offsets from centroid.

### Project Structure (Code Stuff)
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
- An instance based graphics system.
- Wrapper classes around VAO/VBO (Object.h), FBO (RenderPass.h), Shaders (Shader.h), UBO (UniformBuffer.h)
- Usage can mainly be seen in ECS/Systems/RenderSystem.*
- This engine does multi-pass rendering, it requires one set of "main" shaders for our standard rendering, and another set of "screen" shaders to combine the results of each pass into the output we see.
- Object creation (Object.h usage) found in RenderSystem.cpp [457], RenderSystem::CreateShapes()
> Objects refer to meshes such as Sphere, Cube, Line, etc. 
> These meshes are reused both in regular rendering, and debug wireframes.
> The data submitted to GPU can be seen in RenderSystem::Draw, find multiple ".Submit". These specialy submitted data are the instanced data.

Collision (ECS/Systems/CollisionSystem.\*):
- The other bulk of the rubrics are here
- Using a naive O(n^2) double for-loop to check collisions. This will be improved during upcoming BVH project.
- Each test stated by the rubric can be found in each CollisionSystem::CheckCollision function overload.

GSM/GUI (GSM/, GUI/):
- Wrappers around "scripting" and ImGUI windows respectively.
- GSM manages a collection of registered scenes. Each scene provides behaviour for a specific collision test (in the case of this project).
- The ImGUI wrappers provide a way to isolate each window of the "editor" to make things neater.

Utils (Utils/):
- Utility tools such as input, logging, Singleton, event dispatcher

## How to Use
### Key Binds
Move mouse		- Pitch/Yaw FPS camera (While camera toggled)
TAB  (Trigger)	- Toggles FPS camera control
W/A/S/D (Hold)	- Move FPS camera forward/backward/left/right (While camera toggled)
L-SHIFT (Hold)	- Move in -Y
SPACE	(Hold)	- Move in +Y
L-CTRL	(Hold)	- Increase speed while moving (1.5x i think)
F11  (Trigger)	- Toggle window enlarge/shrink
ESC  (Trigger)	- Kill application

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

Settings:
- Random settings such as toggling different passes and FPS

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
- The USE_CSD3151_AUTOMATION for assignment_vs/fs shader buffers are located at the top of RenderSystem.cpp