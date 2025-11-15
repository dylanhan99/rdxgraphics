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
SET(PROJECTS_DIR "${CMAKE_CURRENT_LIST_DIR}/projects")
if (IS_DIRECTORY "${PROJECTS_DIR}")
    file(GLOB children RELATIVE "${PROJECTS_DIR}" "${PROJECTS_DIR}/*")

    # First pass, create all libraries/executables
    foreach (child ${children})
        message("Creating ${child} project...")
        SET(child_source_dir "${PROJECTS_DIR}/${child}")
        set(childType "")
        set(childLibs "")
        set(childDefs "")

        if (EXISTS ${child_source_dir}/config.cmake)
            include(${child_source_dir}/config.cmake)

            if (IS_DIRECTORY "${child_source_dir}")
                file(GLOB_RECURSE child_source_files
                    ${child_source_dir}/*.cpp
                    ${child_source_dir}/*.h
                    ${child_source_dir}/*.hpp
                )

                if (childType STREQUAL "STATIC")
                    add_library(${child} STATIC ${child_source_files} "${PROJECTS_DIR}/RXAPI.h")
                elseif (childType STREQUAL "SHARED")
                    add_library(${child} SHARED ${child_source_files} "${PROJECTS_DIR}/RXAPI.h")
                elseif (childType STREQUAL "EXECUTABLE")
                    add_executable(${child} ${child_source_files} "${PROJECTS_DIR}/RXAPI.h")
                endif()
            endif()
        endif()
    endforeach()

    # Second pass, all linking
    foreach (child ${children})
        message("Linking ${child} project...")
        SET(child_source_dir "${PROJECTS_DIR}/${child}")
        set(childType "")
        set(childLibs "")
        set(childDefs "")

        if (EXISTS ${child_source_dir}/config.cmake)
            include(${child_source_dir}/config.cmake)

            if (IS_DIRECTORY "${child_source_dir}")
                target_link_libraries(${child} PRIVATE ${childLibs})
                target_compile_definitions(${child} PRIVATE ${childLibs})

                if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                    target_compile_options(${child} PRIVATE -Wall ${DisableWarnings})
                elseif (MSVC)
                    target_compile_options(${child} PRIVATE /W3 /WX- /Zc:preprocessor)

                    if (EXISTS "${child_source_dir}/pch.h")
	                    target_precompile_headers(${child} PRIVATE "${child_source_dir}/pch.h")
                    endif()
                endif()

                target_include_directories(${child} PRIVATE ${child_source_dir} ${PROJECTS_DIR})
            endif()
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