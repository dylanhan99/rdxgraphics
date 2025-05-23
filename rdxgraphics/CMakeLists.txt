﻿# CMakeList.txt : CMake project for rdxgraphics, include source and define
# project specific logic here.
#

# Add source to this project's executable.
file(GLOB_RECURSE PCH_REQ_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable (rdxgraphics "${PCH_REQ_SOURCES}")
#glad_add_library(glad_gl_core_33 STATIC API "gl:core=4.5" LANGUAGE c)
imgui_add_library(imgui)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET rdxgraphics PROPERTY CXX_STANDARD 20)
endif()

target_compile_definitions("${PROJECT_NAME}" 
	PUBLIC GLFW_INCLUDE_NONE=1
	PUBLIC GLEW_STATIC
) 
if (RX_IS_IDE_BUILD)
	target_compile_definitions("${PROJECT_NAME}" 
		PRIVATE RX_IS_IDE
		PRIVATE RX_WORKING_DIR="${CMAKE_SOURCE_DIR}"
		PRIVATE RX_HOME
	)
else()
	#target_compile_definitions("${PROJECT_NAME}" 
	#)
endif()

target_include_directories("${PROJECT_NAME}" 
	PUBLIC "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}"
	#INTERFACE "${BS_thread_pool_SOURCE_DIR}/include"
)
#target_link_directories("${PROJECT_NAME}" 
#	#PRIVATE "Vendor/glfw/build/src"
#)
target_link_libraries("${PROJECT_NAME}" 
	PRIVATE glfw
	PRIVATE glm::glm-header-only
	PRIVATE BS_thread_pool
	#PRIVATE glad_gl_core_33
	PRIVATE libglew_static
	PRIVATE imgui
	PRIVATE spdlog::spdlog
	PRIVATE EnTT::EnTT
)