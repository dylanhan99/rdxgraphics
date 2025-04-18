#pragma once

// Standard library
#include <cmath>
#include <crtdbg.h>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <future>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <stdlib.h>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
#include <xhash>
#include <string_view>

// GLAD
#include <glad/gl.h>

// GLFW
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

// BS_thread_pool
#include <BS_thread_pool.hpp>

// imgui
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>


// Local
#include "RXGlobals.h"
#include "Utils/BaseSingleton.h"
#include "Utils/Logger.h"
#include "Utils/EventDispatcher/EventDispatcher.h"