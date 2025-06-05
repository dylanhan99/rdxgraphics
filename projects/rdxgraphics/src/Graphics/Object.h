#pragma once
#include "GraphicsCommon.h"
#include "ECS/Components/BoundingVolume.h"
#define RX_MAX_INSTANCES 1000

//////////////////////////////////////////////////
// How many attrib_type make up value_type?	eg 4*vec4 in a mat4, eg 4*float in a vec4
template <typename T>
static uint32_t GetAttribCount();
#define _RX_SETUP_ATTRIB(Klass, count) template<> static uint32_t GetAttribCount<Klass>() { return count; }
_RX_SETUP_ATTRIB(glm::mat4, 4); // This is 4*vec4, because we have to "wrap" 4*vec4 attribs as one mat4
_RX_SETUP_ATTRIB(glm::vec4, 1); // These are only 1, beacuse they can be directly declared as ONE attrib.
_RX_SETUP_ATTRIB(glm::vec3, 1);
_RX_SETUP_ATTRIB(glm::vec2, 1);
_RX_SETUP_ATTRIB(float, 1);
_RX_SETUP_ATTRIB(uint32_t, 1);
_RX_SETUP_ATTRIB(uint8_t, 1);
#undef _RX_SETUP_ATTRIB
//////////////////////////////////////////////////
template <typename T>
static uint32_t GetFundamentalCount();
#define _RX_SETUP_FUNDA(Klass, count) template<> static uint32_t GetFundamentalCount<Klass>() { return count; }
_RX_SETUP_FUNDA(glm::vec4, 4); // These are only 1, beacuse they can be directly declared as ONE attrib.
_RX_SETUP_FUNDA(glm::vec3, 3);
_RX_SETUP_FUNDA(glm::vec2, 2);
_RX_SETUP_FUNDA(float, 1);
_RX_SETUP_FUNDA(uint32_t, 1);
_RX_SETUP_FUNDA(uint8_t, 1);
#undef _RX_SETUP_FUNDA
//////////////////////////////////////////////////
template <typename T>
static GLenum GetFundamentalType();
#define _RX_SETUP_FUNDA_T(Klass, glType) template<> static GLenum GetFundamentalType<Klass>() { return glType; }
_RX_SETUP_FUNDA_T(glm::vec4, GL_FLOAT);
_RX_SETUP_FUNDA_T(glm::vec3, GL_FLOAT);
_RX_SETUP_FUNDA_T(glm::vec2, GL_FLOAT);
_RX_SETUP_FUNDA_T(float, GL_FLOAT);
_RX_SETUP_FUNDA_T(uint32_t, GL_UNSIGNED_INT);
_RX_SETUP_FUNDA_T(uint8_t, GL_UNSIGNED_BYTE);
#undef _RX_SETUP_FUNDA_T
//////////////////////////////////////////////////

//////////////////////////////////////////////////
#define _RX_SHARED_VERTEX_KLASS(Klass)														\
private:																					\
	inline static uint32_t _IdGen() { static uint32_t i{ 0 }; return i++; }					\
	template <typename T>																	\
	static uint32_t _IdGenTemp() { static uint32_t i = _IdGen(); return i; }				\
public:																						\
	inline static uint32_t Max() { static uint32_t max = _IdGenTemp<Klass>(); return max; }	\
	class BaseAttribute { public: virtual ~BaseAttribute() = default; };
//////////////////////////////////////////////////
#define _RX_ADD_VERTEX(Klass, T, U, isInstanced, isNormalized)								\
	class Klass : public BaseAttribute {													\
		public:																				\
		using value_type = T;																\
		using attrib_type = U;																\
		using container_type = std::vector<T>;												\
		inline static const uint32_t ID{ _IdGenTemp<Klass>() };								\
		inline static const bool IsInstanced{ isInstanced };								\
		inline static const bool IsNormalized{ isNormalized };								\
		private: friend class Vertex; 														\
		public: inline Klass() { ID;}														\
		public: container_type Data{};														\
	}; private: Klass _rx_hack_##Klass{}; public:
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// RXVertex refers to the standard vertex being used by ALL objects in this engine.
class VertexBasic
{
	_RX_SHARED_VERTEX_KLASS(VertexBasic);

	_RX_ADD_VERTEX(Position, glm::vec3, glm::vec3, false, false);
	_RX_ADD_VERTEX(TexCoord, glm::vec2, glm::vec2, false, false);
	_RX_ADD_VERTEX(Normal, glm::vec3, glm::vec3, false, false);
	_RX_ADD_VERTEX(Xform, glm::mat4, glm::vec4, true, false);
	_RX_ADD_VERTEX(Color, glm::vec4, glm::vec4, true, false);
};
// This macro helps to automatically call another macro dubbed "_RX_X". 
// It must meet the usecase
#define RX_VERTEX_BASIC_ATTRIBS_M_NOINSTANCED(F_O_O, ...) \
	F_O_O(VertexBasic::Position,  ##__VA_ARGS__)		  \
	F_O_O(VertexBasic::TexCoord,  ##__VA_ARGS__)		  \
	F_O_O(VertexBasic::Normal,    ##__VA_ARGS__)
#define RX_VERTEX_BASIC_ATTRIBS_M_INSTANCED(F_O_O, ...) \
	F_O_O(VertexBasic::Xform,     ##__VA_ARGS__)	    \
	F_O_O(VertexBasic::Color,  ##__VA_ARGS__)
#define RX_VERTEX_BASIC_ATTRIBS_M(F_O_O, ...) \
	RX_VERTEX_BASIC_ATTRIBS_M_NOINSTANCED(F_O_O, ##__VA_ARGS__) \
	RX_VERTEX_BASIC_ATTRIBS_M_INSTANCED(F_O_O, ##__VA_ARGS__)
#define RX_VERTEX_BASIC_ATTRIBS	\
	RX_VERTEX_BASIC_ATTRIBS_M(_RX_X)
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// As the name states, the vertex used by vertex buffer output.
class VertexFBO
{
	_RX_SHARED_VERTEX_KLASS(VertexFBO);

	_RX_ADD_VERTEX(Position, glm::vec2, glm::vec2, false, false);
	_RX_ADD_VERTEX(TexCoord, glm::vec2, glm::vec2, false, false);
};
// This macro helps to automatically call another macro dubbed "_RX_X". 
// It must meet the usecase
#define RX_VERTEX_FBO_ATTRIBS_M_NOINSTANCED(F_O_O, ...)	\
	F_O_O(VertexFBO::Position, ##__VA_ARGS__)			\
	F_O_O(VertexFBO::TexCoord, ##__VA_ARGS__)
#define RX_VERTEX_FBO_ATTRIBS_M_INSTANCED(F_O_O, ...)
#define RX_VERTEX_FBO_ATTRIBS_M(F_O_O, ...)		 \
	RX_VERTEX_FBO_ATTRIBS_M_NOINSTANCED(F_O_O, ##__VA_ARGS__) \
	RX_VERTEX_FBO_ATTRIBS_M_INSTANCED(F_O_O, ##__VA_ARGS__)
#define RX_VERTEX_FBO_ATTRIBS					 \
	RX_VERTEX_FBO_ATTRIBS_M(_RX_X)
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// Representation of a model
template <typename T>
class Object
{
public:
	using vertex_type = T;

public:
	Object();
	void Terminate();

	void Bind();
	template <typename U>
	std::enable_if_t<std::is_base_of_v<typename T::BaseAttribute, U>,
		void> BindInstancedData(GLsizeiptr offset, GLsizeiptr count);

	Object& BeginObject(GLenum primitive);
	void EndObject();
	Object& PushIndices(std::vector<GLuint> const& indices);
	template <typename U>
	std::enable_if_t<std::is_base_of_v<typename T::BaseAttribute, U>,
		Object&> Push(typename U::container_type const& data);

	template <typename U>
	std::enable_if_t<std::is_base_of_v<typename T::BaseAttribute, U>,
		void> Submit(typename U::value_type val) { GetVBData<U>().emplace_back(std::move(val)); }

	template <typename U>
	std::enable_if_t<std::is_base_of_v<typename T::BaseAttribute, U>,
		GLuint> GetVBO() { return m_VBOs[U::ID]; }

	template <typename U>
	std::enable_if_t<std::is_base_of_v<typename T::BaseAttribute, U>,
		typename U::container_type&> GetVBData() { return std::dynamic_pointer_cast<U>(m_VBData[U::ID])->Data; }

	void Draw(size_t count);
	void Flush();

	inline GLenum GetPrimitive() const { return m_Primitive; }
	inline GLsizei GetPrimCount() const { return m_PrimCount; }
	inline bool IsIndexedMesh() const { return !m_Indices.empty(); }

	inline std::string const& GetName() const { return m_Name; }
	inline void SetName(std::string name) { m_Name = std::move(name); }

	inline AABBBV const& GetDefaultAABBBV() const { return m_DefaultAABBBV; }
	inline AABBBV& GetDefaultAABBBV() { return m_DefaultAABBBV; }

private:
	std::string m_Name{};
	GLuint m_VAO{};
	std::vector<GLuint> m_VBOs{};
	GLuint m_EBO{};
	GLuint m_Index{}; // Attibute index used in the build pattern

	GLenum m_Primitive{};
	GLsizei m_PrimCount{ 0 };
	std::vector<GLuint> m_Indices{};
	std::vector<std::shared_ptr<typename vertex_type::BaseAttribute>> m_VBData{};

	// Pre-calculated default BVs
	AABBBV m_DefaultAABBBV{}; 
};
//////////////////////////////////////////////////

