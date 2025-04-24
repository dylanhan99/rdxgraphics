#pragma once
#define RX_MAX_INSTANCES 1000

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
		inline static const uint32_t ID{ _IdGenTemp<T>() };									\
		inline static const bool IsInstanced{ isInstanced };								\
		inline static const bool IsNormalized{ isNormalized };								\
		private: friend class Vertex; 														\
		public: inline Klass() {ID;}														\
		public: container_type Data{};														\
	}; private: Klass _rx_hack_##Klass{}; public:

//////////////////////////////////////////////////

// RXVertex refers to the standard vertex being used by ALL objects in this engine.
class VertexBasic 
{
	_RX_SHARED_VERTEX_KLASS(VertexBasic);
	
	_RX_ADD_VERTEX(Position, glm::vec3, glm::vec3, false, false);
	_RX_ADD_VERTEX(Xform, glm::mat4, glm::vec4, true, false);
}; 
// This macro helps to automatically call another macro dubbed "_RX_X". 
// It must meet the usecase
#define RX_VERTEX_BASIC_ATTRIBS		\
	_RX_X(VertexBasic::Position);	\
	_RX_X(VertexBasic::Xform);


// As the name states, the vertex used by vertex buffer output.
class VertexFBO
{
	_RX_SHARED_VERTEX_KLASS(VertexFBO);

	_RX_ADD_VERTEX(Position, glm::vec2, glm::vec2, false, false);
	_RX_ADD_VERTEX(TexCoords, glm::vec2, glm::vec2, false, false);
};
// This macro helps to automatically call another macro dubbed "_RX_X". 
// It must meet the usecase
#define RX_VERTEX_FBO_ATTRIBS		\
	_RX_X(VertexFBO::Position);		\
	_RX_X(VertexFBO::TexCoords);


// Representation of a model
template <typename T>
class Object
{
public:
	using vertex_type = T;
	//static bool MakeObject(Object& oObject, GLenum primitive, std::vector<GLuint> const& indices,
	//	std::vector<Vertex::Position::value_type> const& positions);

public:
	Object();
	void Terminate();
	//inline void Submit(glm::mat4 xform = glm::translate(glm::vec3(0.f))) { GetVBData<Vertex::Xform>().emplace_back(std::move(xform)); }

	template <typename T>
	std::enable_if_t<std::is_base_of_v<vertex_type::BaseAttribute, T>,
		void> Submit(typename T::value_type val) { GetVBData<T>().emplace_back(std::move(val)); }

	void Bind();
	//void BindInstancedData();

	template <typename T> 
	std::enable_if_t<std::is_base_of_v<vertex_type::BaseAttribute, T>,
		void> BindInstancedData(GLsizeiptr offset, GLsizeiptr count)
	{
		GLuint vbo = GetVBO<T>();
		typename T::container_type& pData = GetVBData<T>();

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(typename T::value_type), pData.data() + offset, GL_DYNAMIC_DRAW);
	}

	template <typename T>
	GLuint GetVBO() { return m_VBOs[T::ID]; }

	template <typename T>
	typename T::container_type& GetVBData() { return std::dynamic_pointer_cast<T>(m_VBData[T::ID])->Data; }

	//void Draw();
	void Draw(size_t count);

public:
	GLuint m_VAO{};
	std::vector<GLuint> m_VBOs{};
	GLuint m_EBO{};

	GLenum m_Primitive{};
	std::vector<GLuint> m_Indices{};
	std::vector<std::shared_ptr<typename vertex_type::BaseAttribute>> m_VBData{};
};