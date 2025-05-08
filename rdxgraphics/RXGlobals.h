#pragma once

#define RX_UNREF_PARAM(x) (void)x

#define RX_DECLARE_EVENT(x) constexpr const char* x = #x;
RX_DECLARE_EVENT(RX_EVENT_WINDOW_RESIZE);
RX_DECLARE_EVENT(RX_EVENT_FRAMEBUFFER_RESIZE);
RX_DECLARE_EVENT(RX_EVENT_CURSOR_POS_CALLBACK);
RX_DECLARE_EVENT(RX_EVENT_SCROLL_CALLBACK);

RX_DECLARE_EVENT(RX_EVENT_CAMERA_USER_TOGGLED);
#undef RX_DELCARE_EVENT

const std::filesystem::path g_WorkingDir{
#ifdef RX_IS_IDE
	RX_WORKING_DIR
#else
	std::filesystem::current_path()
	// for the csd3151 build, this value is 
	// R"(.....\sample-framework-csd-3151\build)"
	// Hence, shaders are accessed via a backtrack first R"(..\shaders\bla.vert)
#endif
};

#ifdef RX_HOME
#ifdef RX_IS_IDE
#define RX_SHADER_PREFIX RX_WORKING_DIR"/""shaders/"
#endif
#else
#define RX_SHADER_PREFIX "../shaders/"
#endif

const uint64_t RX_INVALID_ID{ 0 };
const glm::vec3 g_WorldUp{ 0.f,1.f,0.f };

enum class Shape
{
	NIL = 0, // Supposed to align with RX_INVALID_ID
	Point,
	Line,
	Quad,
	Plane,
	Cube,
	Sphere,

	//MAX
};

enum class BV
{
	NIL = 0,
	Point,
	Ray,
	Triangle,
	Plane,
	AABB,
	Sphere,

	//MAX
};
