# CS350/CSD3150 | Project 2: Bounding Volumes and View Frustum Culling
By: 2201020 | dylan.h | Han Wei, Dylan</br>
Engine: rdxgraphics

## Project Context
### Environment
- CMake
- git
- Platform Visual Studio 2022 v143 (MSVC)
- C++20

### Libraries
- OpenGL/GLEW/GLFW/glm/ImGUI-docking/ImGuizmo/spdlog/EnTT/assimp/Eigen

### Machines Tested On
- Laptop 1, Windows 11, Intel i7-11800H, NVIDIA GeForce RTX 3060 (Laptop), OpenGL 4.5
- Laptop 2, Windows 10, Intel i7-7500U,  NVIDIA GeForce 940MX			 , OpenGL 4.5
- Lab PC (Multiple), did not take note of specs but all ran fine 		 , OpenGL 4.5

### CMake Changes
Minor edits made to the cmake files provided by the professor. It is likely most of these changes only work for MSVC</br>
- Removed the include/ directory in the project to streamline my own workflow. This includes editing the cmake file to only GLOB_RECURSE over src/ for .h/.hpp/.cpp.
- Added /Zc:preprocessor in MSVC target_compile_options. This allows me to use __VA_OPT__ C++20 preprocessor feature.
- Added setup for other libraries as instructed in CMaksLists.txt and ImportDependencies.cmake. Libraries added include those listed in [Libraries].
- Added target_precompile_headers option to improve compile times
- Added a add_custom_target copy_directory command to copy models/ from root dir to the cmake build/ dir. This ensures that both running from vs2022 and exe directly is the same, but this means you cannot run the exe unless the exe has access to models in ../models.

### Application Setup
- Ensure the software stated in [Environment] are installed
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
- Use [TAB] to toggle control of the FPS camera. Make sure you've clicked on "Viewport" window first, so that the ImGui focus is on that window. That ensures no GUI weirdness occurs when hitting [TAB].
- (Assignment 3) By default, the scene loaded is already the "Assignment 3" scene with multiple entities, each with a different model, with a BVH.
- (Assignment 3) In settings, the most notable setting for the tester would be under "Bounding Volumes", where you'd be able to change the BVH type (TopDown or BottomUp), toggle the visible layers of the BVH, swap current BV option (AABB or Sphere), and change BVH settings such as "Leaf Termination Condition", "Split Point Strategy" and "Node Merging Strategy".
- (Assignment 3) Object picking is implemented with gizmos for the selected entity. You may move the selected entity around to watch the BVH change its form accordingly. 

### Scene Description
Multiple entities, each with different model. The entire BVH is rebuilt every single frame allowing us to watch the BVH form alter to the new changes. This is not ideal performance-wise, but considering the low number of entities, the performance is not impacted.</br>
Just as with using gizmos to move objects around to see the BVH change, you may change the BVH settings under Settings > Bounding Volumes > General/TopDown/BottomUp. Depending on the current BVH type, the TopDown or BottomUp gui tree option will show up.

### Other GUI Features
- Hierarchy allows you to "Create Entity". This adds an entity into the scene.
- You may delete entities by clicking the "X" button next to ecah entity. This feature is disabled for particular entities in "Common" and "Sandbox" scenes.
- In Settings, you may toggle each renderpass (Base, Wireframe, PiP, ...).
- In Settings, under GSM, you may restart a scene to completely reset all entities to the original state.
- In Inspector, for entities with Camera component, you may adjust "Move Speed" if you find the camera too slow.
- In Inspector, for "FPS Cam" entity, you may swap controls between "Regular" or "Arcball". Regular being fps camera controls.

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
- Transformation System (ECS/Systems/TransformSystem) manages both translate/rotate/scale and the recalculation of BVs. The recalculation portion can be found in TransformSystem::Update [77]. They call bv.RecalculateBV(). These RecalculateBV functions can then be found in ECS/Components/BoundingVolume.*. Each BV (Frustum , AABB, OBB, Sphere) have their own definitions for RecalculateBV().

Graphics (Graphics/):</br>
- An instance based graphics system.
- Wrapper classes around VAO/VBO (Object.h), FBO (BasePass.h, Passes/*), Shaders (Shader.h), UBO (UniformBuffer.h)
- Wrapper class around model loading/creation (ObjectFactory.h)
- Usage can mainly be seen in ECS/Systems/RenderSystem.*
- This engine does multi-pass rendering, it requires one set of "main" shaders for our standard rendering, and another set of "screen" shaders to combine the results of each pass into the output we see.
- Object creation (Object/ObjectFactory usage) found in RenderSystem.cpp [199], RenderSystem::CreateShapes()
> Objects refer to meshes such as Sphere, Cube, Line, etc. 
> These meshes are reused both in regular rendering, and debug wireframes.
> The data submitted to GPU can be seen in RenderSystem::Draw, find multiple ".Submit". These specialy submitted data are the instanced data.
> Other meshes loaded using Assimp such as Rhino and Cup are used soley as models (refular rendering)

Collision (ECS/Systems/CollisionSystem.\*):</br>
- Using a naive O(n^2) double for-loop to check collisions.
- Each collision test can be found in each CollisionSystem::CheckCollision function overload.
- View Frustum BV against other entity BVs can be found here in CollisionSystem.cpp [11], CollisionSystem::Update.
- Depending on the collision with the frustum, they are either In/Out/On.
- These BV collision tests (helper funcs) can be found at CollisionSystem.cpp [303], [308], [341], also as CollisionSystem::CheckCollision overloads.

BVH (ECS/Systems/BVHSystem.\*):</br>
- The bulk of the rubrics is here
- The main algorithm for TopDown and BottomUp are BVHSystem::BVHTree_TopDown and BVHSystem::BVHTree_BottomUp.
- All supporting functions such as tree building strategies can be found used within the BVH tree building functions.

GSM/GUI (GSM/, GUI/):</br>
- Wrappers around "scripting" and ImGUI windows respectively.
- GSM manages a collection of registered scenes. Each scene provides behaviour for a specific collision test (in the case of this project).
- The ImGUI wrappers provide a way to isolate each window of the "editor" to make things neater.

Utils (Utils/):</br>
- Utility tools such as input, logging, Singleton, event dispatcher

## How to Use
### Key Binds
Move mouse			  - Pitch/Yaw/Orbit camera (While camera toggled)</br>
TAB			(Trigger) - Toggles camera control</br>
W/A/S/D		   (Hold) - Move FPS camera forward/backward/left/right (While camera toggled)</br>
L-SHIFT		   (Hold) - Move FPS camera in -Y (While camera toggled)</br>
SPACE		   (Hold) - Move FPS camera in +Y (While camera toggled)</br>
L-CTRL		   (Hold) - Increase FPS camera speed while moving (2x speed)</br>
L-ALT + 1	(Trigger) - Change to FPS camera</br>
L-ALT + 2	(Trigger) - Change to Arcball camera</br>
L-ALT + Q	(Trigger) - Change Gizmos to Translate</br>
L-ALT + W	(Trigger) - Change Gizmos to Scale</br>
L-ALT + E	(Trigger) - Change Gizmos to Rotate</br>
F11			(Trigger) - Toggle window enlarge/shrink</br>
ESC			(Trigger) - Kill application</br>

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
- Task 1: Scene Setup					> Complete
- Task 2: BVH Building and Display      > Complete

### Task Code Locations
- Task 1: Assignment3.cpp (Scene setup), ObjectFactory.cpp (Assimp loading), Camera.cpp (Camera)
- Task 2: BVHSystem.cpp (BVH building), BVHPass.cpp (Render pass handling BVH drawing, colors stated there)

## Other Notes
- Average 2 hours daily on this assignment
- The USE_CSD3151_AUTOMATION for assignment_vs/fs shader buffers are located at the top of RenderSystem.cpp
- Do not touch BV in the inspector, only change via Settings window. There is no null validation within BVH system for this and can crash.
- Program crashes in release mode, if objectpicking is attempted with OBB. In debug, no such issue. Unable to determine issue right now.