
class RenderSystem : public BaseSingleton<RenderSystem>
{
	RX_SINGLETON_DECLARATION(RenderSystem);
public:
	static bool Init();
	static void Terminate();

	static void Update(double dt);

	inline static glm::vec3& GetBackBufferColor() { return g.m_BackColor; }

	static bool ReloadShaders();

private:
	static void CreateShapes();

private:
	glm::vec3 m_BackColor{ 0.2f, 0.3f, 0.3f };
	GLuint m_ShaderProgramID{};
};

// Representation of a model
class Object
{
public:
	// _Inst postfix indicates that it's meant to be instanced, hence need subdata/divisor thing
	enum class VertexAttrib {
		Position,
	
		Xform_Inst,
		MAX
	};

	struct Vertex {
		glm::vec3 Position{};
		//glm::vec3 Normal{};
		// ...
	};

	GLuint m_VAO{}, m_EBO{};
	std::array<GLuint, (size_t)VertexAttrib::MAX> m_VBOs{};

	std::vector<GLuint> m_Indices{};

	std::vector<decltype(Vertex::Position)> m_Positions{};
	//std::vector<decltype(Vertex::Normal)> m_Normals{};

	std::vector<glm::mat4> m_Xforms{};

	inline void Submit(glm::mat4 xform = glm::translate(glm::vec3(0.f))) { m_Xforms.emplace_back(std::move(xform)); }
};