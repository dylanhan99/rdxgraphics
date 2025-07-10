#pragma once

#define RX_UNREF_PARAM(x) (void)x

#define RX_DECLARE_EVENT(x) constexpr const char* x = #x;
RX_DECLARE_EVENT(RX_EVENT_WINDOW_RESIZE);
RX_DECLARE_EVENT(RX_EVENT_FRAMEBUFFER_RESIZE);

RX_DECLARE_EVENT(RX_EVENT_CAMERA_USER_TOGGLED);
#undef RX_DELCARE_EVENT

const std::filesystem::path g_WorkingDir{
#ifdef RX_IS_IDE
	RX_WORKING_DIR
#else
	std::filesystem::current_path()
	// for the csd3151 build, this value is 
	// R"(.....\sample-framework-csd-3151\build)"
	// Hence, shaders are accessed via a backtrack first R"(..\projects\projectname\shaders\bla.vert)
#endif
};

#if USE_CSD3151_AUTOMATION == 1
#define RX_PRJ_NAME "dylan.h-project-2"
#else
#define RX_PRJ_NAME "rdxgraphics"
#endif
#define RX_SHADER_PREFIX "../projects/" RX_PRJ_NAME "/shaders/"
#define RX_MODEL_PREFIX "../models/"

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

#define RX_DO_ALL_BVH_ENUM_M(F_O_O, ...)\
	F_O_O(AABB, ##__VA_ARGS__)		\
	F_O_O(Sphere, ##__VA_ARGS__)
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

#define RX_DO_ALL_OBJ_M(F_O_O, ...)		\
	F_O_O(ogre, ##__VA_ARGS__)			\
	F_O_O(bunny, ##__VA_ARGS__)			\
	F_O_O(cup, ##__VA_ARGS__)			\
	F_O_O(head, ##__VA_ARGS__)			\
	F_O_O(lucy_princeton, ##__VA_ARGS__)\
	F_O_O(rhino, ##__VA_ARGS__)			\
	F_O_O(starwars1, ##__VA_ARGS__)		\
	F_O_O(4Sphere, ##__VA_ARGS__)
#define RX_DO_ALL_OBJ RX_DO_ALL_OBJ_M(_RX_X)

#define RX_DO_ALL_UNC_M(F_O_O, ...)				   \
	F_O_O("pp4_a_g0", "UNC/ppsection4/part_a/g0"); \
	F_O_O("pp4_a_g1", "UNC/ppsection4/part_a/g1"); \
	F_O_O("pp4_a_g2", "UNC/ppsection4/part_a/g2"); \
	F_O_O("pp4_b_g0", "UNC/ppsection4/part_b/g0"); \
	F_O_O("pp4_b_g1", "UNC/ppsection4/part_b/g1"); \
	F_O_O("pp5_a_g0", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g1", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g2", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g3", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g4", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g5", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g6", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g7", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g8", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g9", "UNC/ppsection5/part_a/g0"); \
	F_O_O("pp5_a_g10", "UNC/ppsection5/part_a/g10"); \
	F_O_O("pp5_a_g11", "UNC/ppsection5/part_a/g11"); \
	F_O_O("pp5_a_g12", "UNC/ppsection5/part_a/g12"); \
	F_O_O("pp5_a_g13", "UNC/ppsection5/part_a/g13"); \
	F_O_O("pp5_a_g14", "UNC/ppsection5/part_a/g14"); \
	F_O_O("pp5_a_g15", "UNC/ppsection5/part_a/g15"); \
	F_O_O("pp5_a_g16", "UNC/ppsection5/part_a/g16"); \
	F_O_O("pp5_a_g17", "UNC/ppsection5/part_a/g17"); \
	F_O_O("pp5_a_g18", "UNC/ppsection5/part_a/g18"); \
	F_O_O("pp5_a_g19", "UNC/ppsection5/part_a/g19"); \
	F_O_O("pp5_a_g20", "UNC/ppsection5/part_a/g20"); \
	F_O_O("pp5_a_g21", "UNC/ppsection5/part_a/g21"); \
	F_O_O("pp5_a_g22", "UNC/ppsection5/part_a/g22"); \
	F_O_O("pp5_a_g23", "UNC/ppsection5/part_a/g23"); \
	F_O_O("pp5_a_g24", "UNC/ppsection5/part_a/g24"); \
	F_O_O("pp5_a_g25", "UNC/ppsection5/part_a/g25"); \
	F_O_O("pp5_a_g26", "UNC/ppsection5/part_a/g26"); \
	F_O_O("pp5_a_g27", "UNC/ppsection5/part_a/g27"); \
	F_O_O("pp5_a_g28", "UNC/ppsection5/part_a/g28"); \
	F_O_O("pp5_a_g29", "UNC/ppsection5/part_a/g29"); \
	F_O_O("pp5_a_g30", "UNC/ppsection5/part_a/g30"); \
	F_O_O("pp5_a_g31", "UNC/ppsection5/part_a/g31"); \
	F_O_O("pp5_a_g32", "UNC/ppsection5/part_a/g32"); \
	F_O_O("pp5_a_g33", "UNC/ppsection5/part_a/g33"); \
	F_O_O("pp5_a_g34", "UNC/ppsection5/part_a/g34"); \
	F_O_O("pp5_a_g35", "UNC/ppsection5/part_a/g35"); \
	F_O_O("pp5_b_g0", "UNC/ppsection5/part_b/g0"); \
	F_O_O("pp5_b_g1", "UNC/ppsection5/part_b/g1"); \
	F_O_O("pp5_c_g0", "UNC/ppsection5/part_c/g0"); \
	F_O_O("pp5_c_g1", "UNC/ppsection5/part_c/g1"); \
	F_O_O("pp6_a_g0", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g1", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g2", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g3", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g4", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g5", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g6", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g7", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g8", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g9", "UNC/ppsection6/part_a/g0"); \
	F_O_O("pp6_a_g10", "UNC/ppsection6/part_a/g10"); \
	F_O_O("pp6_a_g11", "UNC/ppsection6/part_a/g11"); \
	F_O_O("pp6_a_g12", "UNC/ppsection6/part_a/g12"); \
	F_O_O("pp6_b_g0", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g1", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g2", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g3", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g4", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g5", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g6", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g7", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g8", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g9", "UNC/ppsection6/part_b/g0"); \
	F_O_O("pp6_b_g10", "UNC/ppsection6/part_b/g10"); \
	F_O_O("pp6_b_g11", "UNC/ppsection6/part_b/g11"); \
	F_O_O("pp6_b_g12", "UNC/ppsection6/part_b/g12"); \
	F_O_O("pp6_b_g13", "UNC/ppsection6/part_b/g13"); \
	F_O_O("pp6_b_g14", "UNC/ppsection6/part_b/g14"); \
	F_O_O("pp6_b_g15", "UNC/ppsection6/part_b/g15"); \
	F_O_O("pp6_b_g16", "UNC/ppsection6/part_b/g16"); \
	F_O_O("pp6_b_g17", "UNC/ppsection6/part_b/g17"); \
	F_O_O("pp6_b_g18", "UNC/ppsection6/part_b/g18"); \
	F_O_O("pp6_b_g19", "UNC/ppsection6/part_b/g19"); \
	F_O_O("pp6_b_g20", "UNC/ppsection6/part_b/g20"); \
	F_O_O("pp6_b_g21", "UNC/ppsection6/part_b/g21"); \
	F_O_O("pp6_b_g22", "UNC/ppsection6/part_b/g22");