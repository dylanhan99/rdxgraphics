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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Include project-wide utility and dependency modules
include(ImportDependencies)
importDependencies()

# Global compile flags and macros
#add_definitions(-DGLEW_STATIC)

# Include path for GLM (optional if not using FetchContent)
#include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/glm)

# ======================= PROJECT SETUP =========================
# Auto-discover project folders under /projects
# Stable for Visual Studio (NO RELATIVE PATHS)
set(PROJECTS_DIR "${CMAKE_CURRENT_LIST_DIR}/projects")
if (IS_DIRECTORY "${PROJECTS_DIR}")
    # --- Find all project subfolders (absolute paths, stable in VS) ---
    file(GLOB children LIST_DIRECTORIES TRUE "${PROJECTS_DIR}/*")

    # Containers holding info about each project
    set(ALL_PROJECT_NAMES "")
    set(PROJECT_TYPE "")
    set(PROJECT_LIBS "")
    set(PROJECT_DEFS "")

    # ===============================================
    # FIRST PASS: Create all targets
    # ===============================================
    foreach(child ${children})

        if (IS_DIRECTORY "${child}")

            # Extract the folder name (ProjectA, ProjectB, ...)
            get_filename_component(child_name "${child}" NAME)

            message("Configuring ${child_name}...")

            # Reset values for this project
            set(childType "")
            set(childLibs "")
            set(childDefs "")

            # Load the per-project config if it exists
            if (EXISTS "${child}/config.cmake")
                include("${child}/config.cmake")
            endif()

            # Collect source files
            file(GLOB_RECURSE child_sources
                "${child}/*.cpp"
                "${child}/*.c"
                "${child}/*.h"
                "${child}/*.hpp"
            )

            # RXAPI is common?
            set(common_headers "${PROJECTS_DIR}/RXAPI.h")

            # Create the target based on childType
            if (childType STREQUAL "STATIC")
                add_library(${child_name} STATIC ${child_sources} ${common_headers})
            elseif (childType STREQUAL "SHARED")
                add_library(${child_name} SHARED ${child_sources} ${common_headers})
            elseif (childType STREQUAL "EXECUTABLE")
                add_executable(${child_name} ${child_sources} ${common_headers})
            else()
                message(FATAL_ERROR "Project ${child_name} has no valid childType.")
            endif()

            # Save info for pass #2
            list(APPEND ALL_PROJECT_NAMES ${child_name})
            set(PROJECT_TYPE_${child_name} ${childType})
            set(PROJECT_LIBS_${child_name} ${childLibs})
            set(PROJECT_DEFS_${child_name} ${childDefs})

        endif()
    endforeach()


    # ===============================================
    # SECOND PASS: Linking, include dirs, PCH, options
    # ===============================================
    foreach(child_name ${ALL_PROJECT_NAMES})
        message("Finalizing ${child_name}...")

        set(child "${PROJECTS_DIR}/${child_name}")
        set(childType ${PROJECT_TYPE_${child_name}})
        set(childLibs ${PROJECT_LIBS_${child_name}})
        set(childDefs ${PROJECT_DEFS_${child_name}})

        # --- Linking ---
        if (childType STREQUAL "EXECUTABLE")
            target_link_libraries(${child_name} PRIVATE ${childLibs})
        else()
            target_link_libraries(${child_name} PUBLIC ${childLibs})
        endif()

        # --- Definitions ---
        target_compile_definitions(${child_name} PRIVATE ${childDefs})

        # --- Compiler-specific flags ---
        if (MSVC)
            target_compile_options(${child_name} PRIVATE /W3 /WX- /Zc:preprocessor)

            # Precompiled headers
            if (EXISTS "${child}/pch.h")
                target_precompile_headers(${child_name} PRIVATE "${child}/pch.h")
            endif()

        elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${child_name} PRIVATE -Wall)
        endif()

        # --- Include directories ---
        if (childType STREQUAL "EXECUTABLE")
            target_include_directories(${child_name} PRIVATE ${child} ${PROJECTS_DIR})
        else()
            target_include_directories(${child_name} PUBLIC ${child} ${PROJECTS_DIR})
        endif()

    endforeach()

endif()


# Copy the models/ folder to the output dir
#if(TARGET ${PROJECT_NAME})
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E make_directory
#            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#            "${CMAKE_CURRENT_LIST_DIR}/assets"
#            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
#        COMMENT "Copying assets directory to build output"
#    )
#endif()

add_custom_command(TARGET rdxengine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:rdxengine>
)