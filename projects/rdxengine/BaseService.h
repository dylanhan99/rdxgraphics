#ifndef BASESERVICE_H
#define BASESERVICE_H

namespace rdx
{
	class BaseService
	{
	public:
		virtual ~BaseService() = default;

		bool Init();
		bool Terminate();
		void Update(float dt);
		void Draw();

		inline bool IsInitialized() const { return m_IsInitialized; }
		inline virtual const char* GetName() const { return "Missing Name"; }

	private:
		virtual bool InitImpl() = 0;
		virtual bool TerminateImpl() = 0;

		virtual void UpdateImpl(float dt) {};
		virtual void DrawImpl() {};

	private:
		bool m_IsInitialized{ false };
	};
}

#endif