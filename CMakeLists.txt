# ======================= Project Configuration =========================
cmake_minimum_required(VERSION 2.8...3.13)

# Set the CMake policy version (this enables all policies up to CMake 3.5)
cmake_policy(VERSION 3.5)

project(csd-3151)

# Set the directory where custom CMake modules are stored
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Disabling all install directories to avoid clashes
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE CACHE BOOL "" FORCE)

# Include project-wide utility and dependency modules
include(ImportDependencies)
importDependencies()

# Global compile flags and macros
add_definitions(-DGLEW_STATIC)
add_definitions(-DUSE_CSD3151_AUTOMATION=0) # Used for instructor's automation

# List of external libraries
set(ALL_LIBS
    PRIVATE glfw
    PRIVATE libglew_static
    PRIVATE glm::glm
	PRIVATE imgui
	PRIVATE imguizmo
	PRIVATE spdlog::spdlog
    PRIVATE EnTT::EnTT
	PRIVATE assimp::assimp
    PRIVATE Eigen3::Eigen
)

# Include path for GLM (optional if not using FetchContent)
#include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/glm)

# ======================= AUTO PROJECT DISCOVERY =========================

# Get all subdirectories in the projects folder
file(GLOB children RELATIVE "${CMAKE_CURRENT_LIST_DIR}/projects" "${CMAKE_CURRENT_LIST_DIR}/projects/*")

foreach(child ${children})
    if(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/projects/${child}")
        message(STATUS "Configuring project: ${child}")

        # Collect sources
        file(GLOB_RECURSE ${child}_source_files
            ${CMAKE_CURRENT_LIST_DIR}/projects/${child}/src/*.cpp
            ${CMAKE_CURRENT_LIST_DIR}/projects/${child}/src/*.h
            ${CMAKE_CURRENT_LIST_DIR}/projects/${child}/src/*.hpp
        )

        # Create executable
        add_executable(${child}
            ${${child}_source_files}
        )

        # Link libraries
        target_link_libraries(${child}
            ${ALL_LIBS}
        )

        # Set C++ standard
        set_property(TARGET ${child} PROPERTY CXX_STANDARD 20)

        # Warnings based on compiler
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${child} PRIVATE -Wall ${DisableWarnings})
        elseif (MSVC)
            target_compile_options(${child} PRIVATE /W3 /WX- /Zc:preprocessor)
	    target_precompile_headers(${child} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/projects/${child}/src/pch.h)
        endif()

        # Include path for each project
        target_include_directories(${child} PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/projects/${child}/src
        )
    endif()
endforeach()

# Copy the models/ folder to the output dir
add_custom_target(CopyModels ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/models"
        "${CMAKE_BINARY_DIR}/models"
    COMMENT "Copying models directory to build output"
)

# ======================= END AUTO CONFIGURATION =========================
