# CS350/CSD3150 | Project 1: Geometry Toolbox
By: 2201020 | dylan.h | Han Wei, Dylan
Engine: rdxgraphics

## Project Context
### Environment
- CMake
- git
- Platform Visual Studio 2022 v143 (MSVC)
- C++20

### Libraries
- OpenGL/GLEW/GLFW/glm/ImGUI-docking/spdlog/EnTT

### Machines Tested On
- Laptop 1, Windows 11, Intel i7-11800H, NVIDIA GeForce RTX 3060 (Laptop), OpenGL 4.5
- Laptop 2, Windows 10, Intel i7-7500U,  NVIDIA GeForce 940MX			 , OpenGL 4.5
- Lab PC (Multiple), did not take note of specs but all ran fine 		 , OpenGL 4.5

### CMake Changes
Minor edits made to the cmake files provided by the professor.</br>
- Removed the include/ directory in the project to streamline my own workflow. This includes editing the cmake file to only GLOB_RECURSE over src/ for .h/.hpp/.cpp.
- Added /Zc:preprocessor in MSVC target_compile_options. This allows me to use __VA_OPT__ C++20 preprocessor feature.
- Added setup for other libraries as instructed in CMaksLists.txt and ImportDependencies.cmake. Libraries added include glm, ImGUI, spdlog and EnTT.

### Application Setup
- Ensure the software stated in [Environment] is installed
- Run run.bat the professor provided to build the solution, and the debug/release executables
- In build/Release/ you may find the application executable
- The application runs based on the assumption that USE_CSD3151_AUTOMATION=1 in CMakeLists.txt

### How to use the Application
Context:</br>
- The app has 4 GUI windows - Hierarchy (Left), Viewport (Center-Top), Inspector (Right), Settings (Center-Bottom)
- The Hierarchy, Viewport and Inspector are unity-esque
> Hierarchy showing active entities within each scene, by default we have "Common" and "Sandbox" scenes
> If you select an entity in Hierarchy, the component data can be inspected in Inspector, only Xform, DirectionalLight, Camera and Collider are inspectable right now.
> Viewport shows the view from "FPS Cam" camera entity.
> Settings include various engine settings. 
- In settings, the most notable setting for the tester would be under "GSM", where you'd be able to change scenes. I've created multiple scenes, one for each collision test.
- Use [TAB] to toggle control of the FPS camera 
</br>
Carrying out Collision Tests (For the tester):</br>
Select one of the scenes in Settings > "Select Scene" combo box, eg "Sphere - Sphere".
In Hierarchy, you now see the scene "Sphere - Sphere" with each entity. One of which, being named "(Sphere) Move Me!".
For each of these collision test scenes, the very first entity is the one I'd expect you to play with.
For this sphere, you may move it through Xform component, and play with the collider attributes through Collider component.
</br>
Most of the tests follow the aforementioned instructions, except for those involving Triangle primitives.</br>
Using Point-Triangle as the example, this scene's first entity, the triangle, is special. Because it's calculations are based on the 3 points of the triangle, when either one of these points are moved, the centroid is supposed to update.
Due to the design options of this engine, you *must* disable the "Follow Xform" tag under the Collider > "Triangle Primitive" option. This will ensure that your centroid is updated correctly, when either of the points have been moved.
The reason is that I've set the colliders to follow the xform by default (they have separate positions).

### Other GUI Features
- Hierarchy allows you to "Create Entity". This adds an entity into the scene.
- You may delete entities by clicking the "X" button next to ecah entity. This feature is disabled for particular entities in "Common" and "Sandbox" scenes.
- In Settings, you may toggle each renderpass (Base, Wireframe, Minimap).
- In Settings, under GSM, you may restart a scene in case you messed too much and want to reset.
- In Inspector, Collider component, you may change Primitive types, using the combo box under "Choose Primitive Type".

### Project Structure (Code Stuff)
Main (/):</br>
entrypoint.cpp (main) calls the entry to the application declared in RDX.h/.cpp.
RDX.h/.cpp contains the application-loop.

GLFW (GLFWWindow/):</br>
- Wrapper class around GLFW window functionality. Does the standard initialization, as well as event setups.
- This is the only file which includes glfw headers

ECS (ECS/):</br>
- The project is very much ECS based, so each system (ECS/Systems/*) only touches the components relevant to them.
- Components (ECS/Components/*) are simple datastructures typically only with getter/setter funcitons. Data/lifetime managed by systems.
- EntityManager.h/cpp wraps around EnTT interface.

Graphics (Graphics/):</br>
- An instance based graphics system.
- Wrapper classes around VAO/VBO (Object.h), FBO (RenderPass.h), Shaders (Shader.h), UBO (UniformBuffer.h)
- Usage can mainly be seen in ECS/Systems/RenderSystem.*
- This engine does multi-pass rendering, it requires one set of "main" shaders for our standard rendering, and another set of "screen" shaders to combine the results of each pass into the output we see.
- Object creation (Object.h usage) found in RenderSystem.cpp [457], RenderSystem::CreateShapes()
> Objects refer to meshes such as Sphere, Cube, Line, etc. 
> These meshes are reused both in regular rendering, and debug wireframes.
> The data submitted to GPU can be seen in RenderSystem::Draw, find multiple ".Submit". These specialy submitted data are the instanced data.

Collision (ECS/Systems/CollisionSystem.\*):</br>
- The other bulk of the rubrics are here
- Using a naive O(n^2) double for-loop to check collisions. This will be improved during upcoming BVH project.
- Each test stated by the rubric can be found in each CollisionSystem::CheckCollision function overload.

GSM/GUI (GSM/, GUI/):</br>
- Wrappers around "scripting" and ImGUI windows respectively.
- GSM manages a collection of registered scenes. Each scene provides behaviour for a specific collision test (in the case of this project).
- The ImGUI wrappers provide a way to isolate each window of the "editor" to make things neater.

Utils (Utils/):</br>
- Utility tools such as input, logging, Singleton, event dispatcher

## How to Use
### Key Binds
Move mouse		- Pitch/Yaw FPS camera (While camera toggled)</br>
TAB  (Trigger)	- Toggles FPS camera control</br>
W/A/S/D (Hold)	- Move FPS camera forward/backward/left/right (While camera toggled)</br>
L-SHIFT (Hold)	- Move in -Y (While camera toggled)</br>
SPACE	(Hold)	- Move in +Y (While camera toggled)</br>
L-CTRL	(Hold)	- Increase speed while moving (1.5x i think)</br>
F11  (Trigger)	- Toggle window enlarge/shrink</br>
ESC  (Trigger)	- Kill application</br>

### GUI Guide
Entity Hierarchy:</br>
- Lists entities within each active scene.
- You may select an entity, which allows you to inspect its components.

Inspector:</br>
- Displays the selected entity's components, and the attributes (only Xform, Camera, Directional light, and Collider/BV are supported now)
- Collider/BV's position/xform is separate from Xform component's position/xform. But I made it sync through the TransformaSystem for convenience for now. This can be toggled on/off through the Collider/BV component.
- You may change the Collider's BV on the fly without recompilation.
- There is no [Add Component] function (not yet).

Viewport:</br>
- Displays two camera views by default, FPS (Perspective) and PiP (Ortho).
- Likely need to fullscreen the app to be able to see the PiP on the top right.

Settings:</br>
- Random settings such as changing scenes, toggling different passes, and FPS

## Tasks
Task 1: Window Class								> Complete
Task 2/3: Buffer class VBO and Attribute management > Complete
Task 4: ECS											> Complete
Task 5: Light										> Complete
Task 6: Interactivity (Camera)						> Complete
Collision Tests										> All complete

### Task Code Locations
Task 1:	  GLFWWindow/GLFWWindow.\*, used in RDX.cpp</br>
Task 2/3: Graphics/\*, used mainly in ECS/Systems/RenderSystem.cpp</br>
Task 4:	  ECS/\*, used mainly in ECS/Systems/\*</br>
Task 5: </br>
- ECS/Components/BaseComponent.h, class DirectionalLight
- ECS/Systems/RenderSystem.cpp, submit light component data to shader
- shaders/default.frag			
Task 6:</br>
- ECS/Systems/CollisionSystem.cpp, the full suite (as stated by brief) of CheckCollision(FooBV&, GooBV&) overloads.
- ECS/Components/Colliders.\*, BV components declared here

## Other Notes
- I copy pasted the keymappings from glfw.h into Input.h to mask the same mappings with my own alias instead (RX_ prefix). This is pretty bad.)
- Average 4 hours daily on this assignment
- The USE_CSD3151_AUTOMATION for assignment_vs/fs shader buffers are located at the top of RenderSystem.cpp