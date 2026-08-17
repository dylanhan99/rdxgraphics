# ======================= Project Configuration =========================
cmake_minimum_required(VERSION 3.13)

# Set the CMake policy version (this enables all policies up to CMake 3.5)
cmake_policy(VERSION 3.5)

project(rdxgraphics)

# Enable /MP for all targets globally
if(MSVC)
    add_compile_options(/MP)
endif()

# Disabling all install directories to avoid clashes
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE CACHE BOOL "" FORCE)

# Quarantine Fortran before anyone gets ideas
set(CMAKE_DISABLE_FIND_PACKAGE_LAPACK ON CACHE BOOL "" FORCE)
set(CMAKE_DISABLE_FIND_PACKAGE_BLAS ON CACHE BOOL "" FORCE)
set(CMAKE_Fortran_COMPILER "" CACHE FILEPATH "" FORCE)
set(CMAKE_Fortran_COMPILER_WORKS TRUE CACHE INTERNAL "" FORCE)
set(CMAKE_PROJECT_LANGUAGES C CXX CACHE STRING "" FORCE)

# ======================= Dependencies (vcpkg) =========================
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

# Global compile flags and macros
#add_definitions(-DGLEW_STATIC)
add_definitions(-DUSE_CSD3151_AUTOMATION=0) # Used for instructor's automation

# List of external libraries
set(ALL_LIBS
    PRIVATE glfw
    PRIVATE GLEW::GLEW
    PRIVATE glm::glm
    PRIVATE imgui::imgui
    PRIVATE imguizmo::imguizmo
    PRIVATE spdlog::spdlog
    PRIVATE EnTT::EnTT
    PRIVATE assimp::assimp
    PRIVATE Eigen3::Eigen
)

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
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall
            ${DisableWarnings}
            $<$<CONFIG:Debug>:-Og>
        )
    elseif (MSVC)   
        target_compile_options(${PROJECT_NAME} PRIVATE
            /W3 /Zc:preprocessor
        )
        
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/Od /Zi /RTC1>
            $<$<CONFIG:Release>:/O2 /Oi /Gy /GL>
        )

        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/DEBUG /INCREMENTAL>
            $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>
        )
    endif()
    
    target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FOLDER}/pch.h)

    # Include path for each project
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FOLDER}
    )
endif()

# Copy the assets/ folder to the output dir
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
