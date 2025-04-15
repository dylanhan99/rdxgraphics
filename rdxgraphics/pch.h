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
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// GLM
#include <glm/glm.hpp>

// BS_thread_pool
#include <BS_thread_pool.hpp>

// Local
#include "RXGlobals.h"
#include "Utils/BaseSingleton.h"

#include "Utils/EventDispatcher/EventDispatcher.h"