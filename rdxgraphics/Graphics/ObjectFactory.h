#pragma once
#include "Graphics/Object.h"

#define _RX_X(Klass, ...) typename Klass::container_type,
	using ObjectParams_VertexBasic = std::tuple<GLenum, std::vector<GLuint>,
	RX_VERTEX_BASIC_ATTRIBS_M_NOINSTANCED(_RX_X)
	std::vector<glm::vec3> // Container of precalculated AABB points from assimp (if applicable)
	>;
#undef _RX_X
#define _RX_X(Klass, ...) typename Klass::container_type,
	using ObjectParams_VertexFBO = std::tuple<GLenum, std::vector<GLuint>,
	RX_VERTEX_FBO_ATTRIBS_M_NOINSTANCED(_RX_X)
	void* // Hack to overcome the trailing comma
	>;
#undef _RX_X

// Wrapper for assimp. It's meant to be a helper for Object creation via assimp,
// moved into its own file for neatness
class ObjectFactory
{
public:

public:
	template <typename T, typename U>
	static Object<T> CreateObjekt(U const& objParams);

	// Assimp wrappers

	static ObjectParams_VertexBasic LoadModelFile(std::filesystem::path const& path);
	static ObjectParams_VertexBasic LoadModelBuffer(std::string const& buffer);

	static ObjectParams_VertexFBO CreateScreenQuad();

	static ObjectParams_VertexBasic SetupPoint();
	static ObjectParams_VertexBasic SetupLine();
	static ObjectParams_VertexBasic SetupTriangle();
	static ObjectParams_VertexBasic SetupQuad();
	static ObjectParams_VertexBasic SetupPlane();
	static ObjectParams_VertexBasic SetupCube();
	static ObjectParams_VertexBasic SetupSphere(int stacks = 32, int sectors = 24);
};