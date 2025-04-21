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
#endif
};

struct Vertex {
	// *_inst attributes refer to per-instance attributes
	enum class Attribute 
	{ 
		Position,

		Xform_inst,

		MAX
	};

	using position_type = glm::vec3;
	using xform_type = glm::mat4;

	position_type Position{};
	//glm::vec3 Normal{};
	// ...
	xform_type Xform{};
};

enum class Shape
{
	Point,
	Line,
	Quad,
	Plane,
	Cube,
	Sphere,

	MAX
};
