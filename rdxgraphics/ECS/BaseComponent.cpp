#include <pch.h>
#include "BaseComponent.h"

void Xform::UpdateXform()
{
	m_Xform = glm::translate(m_Translate) * glm::scale(m_Scale);// *glm::rotate(glm::quat(m_Rotate));
}
