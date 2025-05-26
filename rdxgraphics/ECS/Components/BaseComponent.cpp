#include <pch.h>
#include "BaseComponent.h"
#include "ECS/EntityManager.h"

void Xform::UpdateXform()
{
	m_Xform = glm::translate(m_Translate) * glm::scale(m_Scale) * static_cast<glm::mat4>(glm::quat(m_Rotate));
}

glm::vec3& Xform::GetTranslate()
{
	SetDirty();
	return m_Translate;
}

glm::vec3& Xform::GetScale()
{
	SetDirty();
	return m_Scale;
}

glm::vec3& Xform::GetEulerOrientation()
{
	SetDirty();
	return m_Rotate;
}

void Xform::SetTranslate(glm::vec3 t)
{
	SetDirty();
	m_Translate = std::move(t);
}

void Xform::SetScale(glm::vec3 s)
{
	SetDirty();
	m_Scale = std::move(s);
}

void Xform::SetEulerOrientation(glm::vec3 r)
{
	SetDirty();
	m_Rotate = std::move(r);
}

void Xform::SetDirty() const
{
	auto const& handle = GetEntityHandle();
	if (EntityManager::HasComponent<Xform::Dirty>(handle))
		return;

	EntityManager::AddComponent<Xform::Dirty>(handle);
}
