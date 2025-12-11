#ifndef NAIVEBASEPASS_H
#define NAIVEBASEPASS_H
#include "Graphics/MultiPassing/BasePass.h"
#include <gl/glew.h>
#include "Graphics/Rendering/NaiveRenderer/NaiveRenderer.h"

namespace rdx
{
	struct CameraComponent;

	class NaiveBasePass : public BasePass
	{
	public:
		NaiveBasePass(CameraComponent* camera = nullptr) : m_Camera(camera) {};
		~NaiveBasePass() = default;

		bool Init();
		bool Terminate();

		void Draw();

	private:
		virtual bool InitImpl() { return true; }
		virtual bool TerminateImpl() { return true; }
		virtual void DrawImpl() = 0;

	public:
		glm::vec2 m_BufferDims{ 1280.f, 720.f };
		CameraComponent* m_Camera{};

		GLuint m_FBO{};
		GLuint m_TextureBuffer{};
		GLuint m_DepthBuffer{};
	};
}

#endif