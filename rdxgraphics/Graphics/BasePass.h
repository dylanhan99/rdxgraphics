#pragma once
#include "GraphicsCommon.h"

#define _RX_DEF_RENDER_PASS(Klass)								\
public:															\
	inline Klass(												\
		std::string const& displayName,							\
		std::string const& handleName,							\
		bool defaultEnabled = true)								\
		: BasePass(displayName, handleName, defaultEnabled) {}	\
private:

class BasePass
{
public:
	BasePass() = delete;
	~BasePass() = default;

	// handleName => "Wireframe", then in screenfrag shader, it would look for
	// the appended "uHas___" => "uHasWireframe" for example. This is the boolean to draw this frame or not
	inline BasePass(std::string const& displayName, std::string const& handleName, bool defaultEnabled)
		: m_DisplayName(displayName), 
		m_HandleTexName("u" + handleName + "Tex"),
		m_HasHandleName("uHas" + handleName),
		m_Enabled(defaultEnabled) {}

	bool Init(void*); // For final pass, no fbo, direct to screen.
	bool Init(int x, int y, int width, int height);
	void Terminate();

	void Draw() const;
	virtual void DrawImpl() const = 0;

	inline GLuint GetTextureBuffer() const { return m_TextureBuffer; }
	inline glm::ivec2 GetViewportDims() const { return m_ViewportDims; }
	inline glm::ivec2 GetBufferDims() const { return m_BufferDims; }
	inline glm::vec4& GetBackbufferColor() { return m_BackbufferColor; }

	inline std::string const& GetDisplayName() const { return m_DisplayName; }
	inline std::string const& GetHandleTexName() const { return m_HandleTexName; }
	inline std::string const& GetHasHandleName() const { return m_HasHandleName; }

	inline bool IsEnabled() const { return m_Enabled; }
	inline bool& IsEnabled() { return m_Enabled; }

private:
	std::string const m_DisplayName{};
	std::string const m_HandleTexName{}, m_HasHandleName{};
	bool m_Enabled{};

	GLuint m_FBO{};
	GLuint m_TextureBuffer{};
	GLuint m_DepthBuffer{};

	glm::ivec2 m_ViewportPos{}, m_ViewportDims{};
	glm::ivec2 m_BufferPos{}, m_BufferDims{};

	glm::vec4 m_BackbufferColor{ 0.2f, 0.3f, 0.3f, 0.f };
};