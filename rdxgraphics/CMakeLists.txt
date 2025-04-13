# CMakeList.txt : CMake project for rdxgraphics, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (rdxgraphics "rdxgraphics.cpp" "rdxgraphics.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET rdxgraphics PROPERTY CXX_STANDARD 20)
endif()

target_compile_definitions("${PROJECT_NAME}" 
	PUBLIC GLFW_INCLUDE_NONE=1
) 
target_include_directories("${PROJECT_NAME}" 
	PUBLIC Vendor/glfw/include
)
target_link_directories("${PROJECT_NAME}" 
	PRIVATE Vendor/glfw/build/src
)
target_link_libraries("${PROJECT_NAME}" 
	PRIVATE glfw
)