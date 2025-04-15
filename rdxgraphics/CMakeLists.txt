# CMakeList.txt : CMake project for rdxgraphics, include source and define
# project specific logic here.
#

# Add source to this project's executable.
file(GLOB_RECURSE PCH_REQ_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable (rdxgraphics "${PCH_REQ_SOURCES}")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET rdxgraphics PROPERTY CXX_STANDARD 20)
endif()

target_compile_definitions("${PROJECT_NAME}" 
	PUBLIC GLFW_INCLUDE_NONE=1
) 
target_include_directories("${PROJECT_NAME}" 
	PUBLIC "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}"
	#PUBLIC "Vendor/glfw/include"
	#PUBLIC "Vendor/glm"
)
#target_link_directories("${PROJECT_NAME}" 
#	#PRIVATE "Vendor/glfw/build/src"
#)
target_link_libraries("${PROJECT_NAME}" 
	#PRIVATE glfw
	#PRIVATE glm::glm-header-only
	PRIVATE glfw glm::glm-header-only
)