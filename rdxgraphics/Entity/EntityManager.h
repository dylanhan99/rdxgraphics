#pragma once
#include <Collision/BoundingVolume.h>

class Entity
{
public:
	struct ModelDetails // Data describing base model/mesh
	{
		glm::vec3 Translate{}, Scale{}, Rotate{};
		glm::mat4 Xform{};

		Shape ShapeType{ Shape::Quad };
		void UpdateXform();
	};

	struct ColliderDetails // Data describing collider
	{
		BV BVType{ BV::NIL };
		std::shared_ptr<BaseBoundingVolume> pBV{ nullptr };

		inline void UpdateXform() const { if (pBV) pBV->UpdateXform(); }
	};

public:
	inline ModelDetails& GetModelDetails() { return m_ModelDetails; }
	inline ColliderDetails& GetColliderDetails() { return m_ColliderDetails; }

private:
	ModelDetails m_ModelDetails;
	ColliderDetails m_ColliderDetails;
};

class EntityManager : public BaseSingleton<EntityManager>
{
	RX_SINGLETON_DECLARATION(EntityManager);
public:
	static bool Init();
	static void Terminate();

	inline static std::vector<Entity>& GetEntities() { return g.m_Entities; }

private:
	std::vector<Entity> m_Entities{};
};