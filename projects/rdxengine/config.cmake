set(childType "SHARED")
set(childLibs 
	glfw
    libglew_static
    glm::glm
	spdlog::spdlog
    EnTT::EnTT
	assimp::assimp
    Eigen3::Eigen
)
set(childDefs
	GLEW_STATIC
	RX_EXPORTS
)