#pragma once
#include "Collision/BoundingVolume.h"
#include "BaseComponent.h"

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

	template <typename T, typename ...Args>
	bool AddComponent(Args&& ...args)
	{

	}

	template <typename T>
	bool RemoveComponent()
	{

	}

public:
	template <typename T, typename ...Args>
	std::enable_if_t<std::is_constructible_v<T, Args...>,
		void> SetCollider(Args&& ...args)
	{
		auto& colDets = GetColliderDetails();
		colDets.BVType = T::BVType;
		colDets.pBV = std::make_shared<T>(std::forward(args)...);
	}

	inline ModelDetails& GetModelDetails() { return m_ModelDetails; }
	inline ColliderDetails& GetColliderDetails() { return m_ColliderDetails; }

private:
	ModelDetails m_ModelDetails;
	ColliderDetails m_ColliderDetails;
};
