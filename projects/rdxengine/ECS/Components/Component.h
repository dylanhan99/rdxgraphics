#ifndef COMPONENT_H
#define COMPONENT_H
#include "rxtypes.h"

namespace rdx
{
	inline ComponentID GenerateComponentID()
	{
		static ComponentID counter = 0;
		return counter++;
	}

	template <typename T>
	ComponentID GetComponentID()
	{
		static ComponentID typeID = GenerateComponentID();
		return typeID;
	}

	// Juust an example
	struct TransformComponent // No base class
	{
		// ...
		glm::vec3 Position{1.f, 0.f, 0.f};
	};
}

#endif