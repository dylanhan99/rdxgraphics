include(FetchContent)

# Macro to import GLFW
macro(import_glfw)
    if(NOT TARGET glfw)  # Guard to prevent multiple inclusion
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.3.8
        )
        if(NOT glfw_POPULATED)
            set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
            set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
            set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
            set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
            FetchContent_Populate(glfw)
        endif()

        add_subdirectory(${glfw_SOURCE_DIR})
        include_directories(${GLFW_SOURCE_DIR}/include)
    endif()
endmacro()

# Macro to import glm
macro(import_glm)
    if(NOT TARGET glm)  # Guard to prevent multiple inclusion
        FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG 1.0.1
        )
        set(GLM_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(glm)

        include_directories(${glm_SOURCE_DIR})
    endif()
endmacro()

# Macro to import glew
macro(import_glew)
    if(NOT TARGET glew_s)  # Guard to prevent multiple inclusion
        include(FetchContent)

        FetchContent_Declare(
            glew
            GIT_REPOSITORY https://github.com/omniavinco/glew-cmake.git
            GIT_TAG master  # Or use a specific commit or tag if preferred
        )

        # Disable shared builds if needed
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libs" FORCE)
        FetchContent_MakeAvailable(glew)

        # Optional: include directories if needed manually (though target_link_libraries handles it)
        include_directories(${glew_SOURCE_DIR}/include)
    endif()
endmacro()

#macro to import spdlog
macro(import_spdlog)
    if(NOT TARGET spdlog)
        include(FetchContent)

        FetchContent_Declare(spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG        v1.15.2
        )
        set(SPDLOG_ENABLE_PCH   ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(spdlog)
    endif()
endmacro()

macro(import_imgui)
    if(NOT TARGET imgui)
        include(FetchContent)

        FetchContent_Declare(imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG        v1.91.9b-docking
        )
        FetchContent_MakeAvailable(imgui)

        # include_directories(${glew_SOURCE_DIR}/include)
        file(GLOB IM_SOURCES_0 CONFIGURE_DEPENDS
            "${imgui_SOURCE_DIR}/*.cpp"
        )
        file(GLOB IM_SOURCES_1 CONFIGURE_DEPENDS
            "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
            "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
        )

        set(IMGUI_REQ_SOURCES ${IM_SOURCES_0} ${IM_SOURCES_1})
        list(REMOVE_ITEM IMGUI_REQ_SOURCES "${imgui_SOURCE_DIR}/imgui_demo.*")

        #message(STATUS "IMGUI_REQ_SOURCES > ${IMGUI_REQ_SOURCES}")

        add_library(imgui STATIC "${IMGUI_REQ_SOURCES}")
        target_include_directories(imgui
	        PUBLIC "${imgui_SOURCE_DIR}"
	        PUBLIC "${imgui_SOURCE_DIR}/backends/"
        )

        target_link_libraries(imgui
	        PRIVATE glfw
        )
    endif()
endmacro()

#macro to import entt
macro(import_entt)
    if(NOT TARGET entt)
        include(FetchContent)

        FetchContent_Declare(entt
            GIT_REPOSITORY https://github.com/skypjack/entt.git
            GIT_TAG        v3.15.0
        )
        FetchContent_MakeAvailable(entt)
    endif()
endmacro()

#macro to import assimp
macro(import_assimp)
    if(NOT TARGET assimp)
        include(FetchContent)

        FetchContent_Declare(assimp
            GIT_REPOSITORY https://github.com/assimp/assimp.git
            GIT_TAG        v5.4.3
        )

        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libs" FORCE)
        set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
        set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(assimp)
    endif()
endmacro()

#macro to import eigen
macro(import_eigen)
    if(NOT TARGET eigen)
        include(FetchContent)

        FetchContent_Declare(eigen
            GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
            GIT_TAG        3.4.0
        )
        set(EIGEN_INSTALL OFF CACHE BOOL "" FORCE)
        set(EIGEN_BUILD_UNINSTALL_TARGET OFF CACHE BOOL "" FORCE)
        set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(eigen)
    endif()
endmacro()

# Macro to import all dependencies
macro(importDependencies)
    message(STATUS "Starting to import dependencies...")

    message(STATUS "Importing GLFW...")
    import_glfw()
    message(STATUS "GLFW imported successfully.")

    message(STATUS "Importing GLM...")
    import_glm()
    message(STATUS "GLM imported successfully.")

    message(STATUS "Importing GLEW...")
    import_glew()
    message(STATUS "GLEW imported successfully.")

    message(STATUS "Importing spdlog...")
    import_spdlog()
    message(STATUS "spdlog imported successfully.")

    message(STATUS "Importing imgui-docking...")
    import_imgui()
    message(STATUS "imgui-docking imported successfully.")

    message(STATUS "Importing entt...")
    import_entt()
    message(STATUS "entt imported successfully.")

    message(STATUS "Importing assimp...")
    import_assimp()
    message(STATUS "assimp imported successfully.")

    message(STATUS "Importing eigen...")
    import_eigen()
    message(STATUS "eigen imported successfully.")

    message(STATUS "All dependencies have been imported successfully.")
endmacro()