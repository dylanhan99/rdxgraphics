#include <pch.h>
#include "Entity.h"

void Entity::ModelDetails::UpdateXform()
{
	Xform = glm::mat4(glm::translate(Translate));
}
