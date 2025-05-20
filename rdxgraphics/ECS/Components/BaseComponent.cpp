#include <pch.h>
#include "BaseComponent.h"
#include "ECS/EntityManager.h"

void Xform::UpdateXform()
{
	m_Xform = glm::translate(m_Translate) * glm::scale(m_Scale) * static_cast<glm::mat4>(glm::quat(m_Rotate));
}
