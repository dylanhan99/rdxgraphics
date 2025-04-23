#pragma once
#define RX_MAX_INSTANCES 1000

// This macro helps to automatically call another macro dubbed "_RX_X". 
// It must meet the usecase
#define RX_VERTEX_ATTRIBS	 \
	_RX_X(Vertex::Position); \
	_RX_X(Vertex::Xform);

class Vertex {
private:
	inline static uint32_t _IdGen() { static uint32_t i{ 0 }; return i++; }
	template <typename T>
	static uint32_t _IdGenTemp() { static uint32_t i = _IdGen(); return i; }

public:
	inline static uint32_t Max() { static uint32_t max = _IdGenTemp<Vertex>(); return max; }

	class BaseAttribute { public: virtual ~BaseAttribute() = default; };
#define _RX_ADD_VERTEX(Klass, T, isInstanced, isNormalized)		\
	class Klass : public BaseAttribute {						\
		public:													\
		using value_type = T;									\
		using container_type = std::vector<T>;					\
		inline static const uint32_t ID{ _IdGenTemp<T>() };		\
		inline static const bool IsInstanced{ isInstanced };	\
		inline static const bool IsNormalized{ isNormalized };	\
		private: friend class Vertex; 							\
		public: inline Klass() {ID;}							\
		public: container_type Data{};							\
	}; private: Klass _rx_hack_##Klass{}; public:
	
	_RX_ADD_VERTEX(Position, glm::vec3, false, false);
	_RX_ADD_VERTEX(Xform, glm::mat4, true, false);

#undef _RX_ADD_VERTEX
}; 

// Representation of a model
class Object
{
public:
	static bool MakeObject(Object& oObject, GLenum primitive, std::vector<GLuint> const& indices,
		std::vector<Vertex::Position::value_type> const& positions);

public:
	Object();
	void Terminate();
	inline void Submit(glm::mat4 xform = glm::translate(glm::vec3(0.f))) { GetVBData<Vertex::Xform>().emplace_back(std::move(xform)); }

	void Bind();
	//void BindInstancedData();

	template <typename T> 
	std::enable_if_t<std::is_base_of_v<Vertex::BaseAttribute, T>,
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

	//inline std::vector<glm::mat4>& GetXForms() { return m_Xforms; }

public:
	GLuint m_VAO{};
	std::vector<GLuint> m_VBOs{};
	GLuint m_EBO{};

	GLenum m_Primitive{};
	std::vector<GLuint> m_Indices{};
	std::vector<std::shared_ptr<Vertex::BaseAttribute>> m_VBData{};
	//std::vector<Vertex::Position::value_type> m_Positions{};
	//std::vector<Vertex::Xform::value_type> m_Xforms{};
};