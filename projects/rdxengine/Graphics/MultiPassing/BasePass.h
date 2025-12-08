#ifndef BASEPASS_H
#define BASEPASS_H

namespace rdx
{
	class BasePass
	{
	public:
		~BasePass() = default;

		void Draw();
	private:
		virtual void DrawImpl() = 0;
	};
}

#endif