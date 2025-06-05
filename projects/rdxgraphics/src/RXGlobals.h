#pragma once

#define RX_UNREF_PARAM(x) (void)x

#define RX_DECLARE_EVENT(x) constexpr const char* x = #x;
RX_DECLARE_EVENT(RX_EVENT_WINDOW_RESIZE);
RX_DECLARE_EVENT(RX_EVENT_FRAMEBUFFER_RESIZE);
RX_DECLARE_EVENT(RX_EVENT_CURSOR_POS_CALLBACK);

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

#ifdef USE_CSD3151_AUTOMATION
#if USE_CSD3151_AUTOMATION == 1
#define RX_SHADER_PREFIX "../shaders/"
#define RX_MODEL_PREFIX "../models/"
#else
#define RX_SHADER_PREFIX "../../../projects/rdxgraphics/shaders/"
#define RX_MODEL_PREFIX "../../../projects/rdxgraphics/models/"
#endif
#endif

const uint64_t RX_INVALID_ID{ 0 };
const glm::vec3 g_WorldUp{ 0.f,1.f,0.f };

#define RX_DO_ALL_SHAPE_ENUM_M(F_O_O, ...)\
	F_O_O(Point, ##__VA_ARGS__)			\
	F_O_O(Line, ##__VA_ARGS__)			\
	F_O_O(Triangle, ##__VA_ARGS__)		\
	F_O_O(Quad, ##__VA_ARGS__)			\
	F_O_O(Plane, ##__VA_ARGS__)			\
	F_O_O(Cube, ##__VA_ARGS__)			\
	F_O_O(Sphere, ##__VA_ARGS__)
#define RX_DO_ALL_SHAPE_ENUM RX_DO_ALL_SHAPE_ENUM_M(_RX_X)
#define RX_DO_ALL_SHAPE_ENUM_AND_NIL _RX_X(NIL) RX_DO_ALL_SHAPE_ENUM_M(_RX_X)
#define _RX_X(Klass) Klass,
enum class Shape
{
	RX_DO_ALL_SHAPE_ENUM_AND_NIL
};
#undef _RX_X

// Duplicated RX_DO_ALL_BV_ENUM_M becasue msvc is screaming at me because of my "deeply nested" macro usage.
// see CollisionSystem for context
#define RX_DO_ALL_PRIMITIVE_ENUM_M(F_O_O, ...)\
	F_O_O(Point, ##__VA_ARGS__)			\
	F_O_O(Ray, ##__VA_ARGS__)			\
	F_O_O(Triangle, ##__VA_ARGS__)		\
	F_O_O(Plane, ##__VA_ARGS__)			\
	F_O_O(AABB, ##__VA_ARGS__)			\
	F_O_O(Sphere, ##__VA_ARGS__)
#define RX_DO_ALL_PRIMITIVE_ENUM_M_(F_O_O, ...)\
	F_O_O(Point, ##__VA_ARGS__)			\
	F_O_O(Ray, ##__VA_ARGS__)			\
	F_O_O(Triangle, ##__VA_ARGS__)		\
	F_O_O(Plane, ##__VA_ARGS__)			\
	F_O_O(AABB, ##__VA_ARGS__)			\
	F_O_O(Sphere, ##__VA_ARGS__)
#define RX_DO_ALL_PRIMITIVE_ENUM RX_DO_ALL_PRIMITIVE_ENUM_M(_RX_X)
#define RX_DO_ALL_PRIMITIVE_ENUM_AND_NIL _RX_X(NIL) RX_DO_ALL_PRIMITIVE_ENUM_M(_RX_X)
#define _RX_X(Klass) Klass,
enum class Primitive
{
	RX_DO_ALL_PRIMITIVE_ENUM_AND_NIL
};
#undef _RX_X

#define RX_DO_ALL_BV_ENUM_M(F_O_O, ...)\
	F_O_O(Frustum, ##__VA_ARGS__)		\
	F_O_O(AABB, ##__VA_ARGS__)			\
	F_O_O(OBB, ##__VA_ARGS__)			\
	F_O_O(Sphere, ##__VA_ARGS__)
#define RX_DO_ALL_BV_ENUM RX_DO_ALL_BV_ENUM_M(_RX_X)
#define RX_DO_ALL_BV_ENUM_AND_NIL _RX_X(NIL) RX_DO_ALL_BV_ENUM_M(_RX_X)
#define _RX_X(Klass) Klass,
enum class BV
{
	RX_DO_ALL_BV_ENUM_AND_NIL
};
#undef _RX_X
