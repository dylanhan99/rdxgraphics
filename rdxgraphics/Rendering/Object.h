#pragma once
#define RX_MAX_INSTANCES 1000

class Vertex {
private:
	inline static uint32_t _IdGen() { static uint32_t i{ 0 }; 
	std::cout << i << "\n"; 
	return i++; }
	template <typename T>
	static uint32_t _IdGenTemp() { static uint32_t i = _IdGen(); return i; }

public:
	inline static uint32_t Max() { static uint32_t max = _IdGenTemp<Vertex>(); return max; }
	inline Vertex()
	{
		std::cout << 'a';
		std::cout << Vertex::Max();
	}
	class BaseAttribute {};
#define _RX_ADD_VERTEX(Klass, T, isInstanced, isNormalized)		\
	class Klass : public BaseAttribute {						\
		public:													\
		using value_type = T;									\
		inline static const uint32_t ID{ _IdGenTemp<T>() };		\
		inline static const bool IsInstanced{ isInstanced };	\
		inline static const bool IsNormalized{ isNormalized };	\
		private: friend class Vertex; inline Klass() {ID;}		\
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
	inline Object() { m_VBOs.resize(Vertex::Max()); }
	void Terminate();
	inline void Submit(glm::mat4 xform = glm::translate(glm::vec3(0.f))) { m_Xforms.emplace_back(std::move(xform)); }

	void Bind();
	void BindInstancedData();

	template <typename T> 
	std::enable_if_t<std::is_base_of_v<Vertex::BaseAttribute, T>,
		void> BindInstancedData(GLsizeiptr count, void* pData)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[T::ID]);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(T::value_type), pData, GL_DYNAMIC_DRAW);
	}

	void Draw();

	inline std::vector<glm::mat4>& GetXForms() { return m_Xforms; }

public:
	GLuint m_VAO{};
	std::vector<GLuint> m_VBOs{};
	GLuint m_EBO{};

	GLenum m_Primitive{};

	std::vector<GLuint> m_Indices{};
	std::vector<Vertex::Position::value_type> m_Positions{};
	std::vector<Vertex::Xform::value_type> m_Xforms{};
};