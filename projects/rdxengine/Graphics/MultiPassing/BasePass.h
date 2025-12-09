#ifndef BASEPASS_H
#define BASEPASS_H
#include <gl/glew.h>

namespace rdx
{
	class BasePass
	{
	public:
		~BasePass() = default;

		bool Init();
		bool Terminate();

		void Draw();

	private:
		virtual void DrawImpl() = 0;

	public:
		glm::vec2 m_BufferDims{ 1280.f, 720.f };

		GLuint m_FBO{};
		GLuint m_TextureBuffer{};
		GLuint m_DepthBuffer{};
	};
}

#endif