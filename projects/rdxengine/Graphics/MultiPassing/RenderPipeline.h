#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H
#include "BasePass.h"

namespace rdx
{
	struct RenderPipeline
	{
		void RegisterPass(std::weak_ptr<BasePass> pass)
		{
			Passes.emplace_back(pass);
		}

		std::vector<std::weak_ptr<BasePass>> Passes{};
	};
}

#endif