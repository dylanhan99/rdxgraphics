cmake_minimum_required(VERSION 3.8)
find_package(Python3 REQUIRED)

option(RX_IS_IDE_BUILD "Enable IDE debugger-specific settings" ON)

project(rdxgraphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer static runtime for MSVC
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(MSVC)
    add_compile_options(/permissive- /Zc:__cplusplus /Zc:preprocessor)
endif()

include(FetchContent)

set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

set(GLM_BUILD_TESTS     OFF CACHE BOOL "" FORCE)

set(SPDLOG_ENABLE_PCH   ON CACHE BOOL "" FORCE)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libs" FORCE)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)


FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

FetchContent_Declare(BS_thread_pool
    GIT_REPOSITORY https://github.com/bshoshany/thread-pool.git
    GIT_TAG        v5.0.0
)

#FetchContent_Declare(glad
#    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
#    GIT_TAG        f49229523c58ad7698e941f7c51454e77dd9d6a8
#    SOURCE_SUBDIR  "cmake"
#)

FetchContent_Declare(glew
    GIT_REPOSITORY https://github.com/omniavinco/glew-cmake.git
    GIT_TAG master  # Or use a specific commit or tag if preferred
)

FetchContent_Declare(imgui
    #GIT_REPOSITORY https://github.com/ocornut/imgui.git
    #GIT_TAG        v1.91.9b-docking
    GIT_REPOSITORY https://github.com/dylanhan99/imgui.git
    GIT_TAG        docking
    SOURCE_SUBDIR  "cmake"
)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.2
)

FetchContent_Declare(entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v3.15.0
)

FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.3
)

#########################################################
# Additional setup for BS_thread_pool. Header-only, no cmakelists
FetchContent_GetProperties(BS_thread_pool)
if(NOT bs_thread_pool_POPULATED)
    FetchContent_Populate(BS_thread_pool)

    # Create a header-only interface target manually
    add_library(BS_thread_pool INTERFACE)
    target_include_directories(BS_thread_pool INTERFACE
        "${bs_thread_pool_SOURCE_DIR}/include"
    )
endif()
#########################################################


#########################################################
#########################################################
# Only MakeAvailable projects who have cmakelists file
#FetchContent_MakeAvailable(glfw glm glad imgui spdlog)
FetchContent_MakeAvailable(glfw glm glew imgui spdlog entt assimp)

# Add your actual project
add_subdirectory(rdxgraphics)