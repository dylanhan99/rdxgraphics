# ======================= Project Configuration =========================
cmake_minimum_required(VERSION 3.13)

# Set the CMake policy version (this enables all policies up to CMake 3.5)
cmake_policy(VERSION 3.5)

project(rdxgraphics)

# Enable /MP for all targets globally
if(MSVC)
    add_compile_options(/MP)
endif()

# Set the directory where custom CMake modules are stored
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Disabling all install directories to avoid clashes
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE CACHE BOOL "" FORCE)
# Quarantine Fortran before anyone gets ideas
set(CMAKE_DISABLE_FIND_PACKAGE_LAPACK ON CACHE BOOL "" FORCE)
set(CMAKE_DISABLE_FIND_PACKAGE_BLAS ON CACHE BOOL "" FORCE)
set(CMAKE_Fortran_COMPILER "" CACHE FILEPATH "" FORCE)
set(CMAKE_Fortran_COMPILER_WORKS TRUE CACHE INTERNAL "" FORCE)
set(CMAKE_PROJECT_LANGUAGES C CXX CACHE STRING "" FORCE)

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

# ======================= PROJECT SETUP =========================
SET(SOURCE_FOLDER "src")

if(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FOLDER}")
    message(STATUS "Configuring project: ${PROJECT_NAME}")

    # Collect sources
    file(GLOB_RECURSE ${PROJECT_NAME}_source_files
        ${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FOLDER}/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FOLDER}/*.h
        ${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FOLDER}/*.hpp
    )

    # Create executable
    add_executable(${PROJECT_NAME}
        ${${PROJECT_NAME}_source_files}
    )

    # Link libraries
    target_link_libraries(${PROJECT_NAME}
        ${ALL_LIBS}
    )

    # Set C++ standard
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

    # Warnings based on compiler
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall ${DisableWarnings})
    elseif (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX- /Zc:preprocessor)
	    target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FOLDER}/pch.h)
    endif()

    # Include path for each project
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FOLDER}
    )
endif()

# Copy the models/ folder to the output dir
if(TARGET ${PROJECT_NAME})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_LIST_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copying assets directory to build output"
    )
endif()