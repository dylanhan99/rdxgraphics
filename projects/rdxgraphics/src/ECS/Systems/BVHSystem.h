#pragma once
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

struct BVHNode
{
	struct TypeNode : public BaseComponent { char _{}; };
	struct TypeLeaf : public BaseComponent { char _{}; };

	entt::entity Handle{ entt::null }; // Handle to this node's bounding bolume
	std::unique_ptr<BVHNode> Left{nullptr};
	std::unique_ptr<BVHNode> Right{nullptr};
	std::vector<entt::entity> Objects{}; // If it's a leaf node, this should have >= MinObjectsPerLeaf entity handles

	void SetIsNode() const;
	void SetIsLeaf() const;
	bool IsLeaf() const;
};

class BVHSystem : public BaseSingleton<BVHSystem>
{
	RX_SINGLETON_DECLARATION(BVHSystem);
public:
	enum class BVHType {
		TopDown,
	};
	enum class BVHAxis {
		X,
		Y,
		Z,
		PCA
	};

private:
	using Entity = std::pair<entt::entity, float>; // handle, dotproduct on axis
	using EntityList = std::vector<Entity>;

public:
	static bool Init();
	static void EnforceUniformBVs();
	inline static std::unique_ptr<BVHNode>& GetRootNode() { return g.m_RootNode; }
	inline static BV& GetGlobalBVType() { return g.m_GlobalBVType; }
	inline static BVHType& GetCurrentTreeType() { return g.m_CurrentTreeType; }
	inline static BVHAxis& GetCurrentTreeAxis() { return g.m_CurrentTreeAxis; }

	// *** Helper functions *** //
	static void BuildBVH();
	static void BuildBVH(std::function<void(std::unique_ptr<BVHNode>&, Entity*, int, int)> fnBuildBVH);
	static void DestroyBVH(std::unique_ptr<BVHNode>& pNode);

	// Determines split plane
	static glm::vec3 GetTreeAxis(std::vector<glm::vec3> const& positions);
	static EntityList GetSortedEntities(std::vector<entt::entity> const& entities);
	static int Partition(Entity* pEntities, int numEnts);
	// *** *** //

	// *** Tree building *** //
	static void BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* pEntities, int numEnts, int height = 0);
	// *** *** //

private:

private:
	BV m_GlobalBVType{ BV::AABB };
	BVHType m_CurrentTreeType{ BVHType::TopDown };
	BVHAxis m_CurrentTreeAxis{ BVHAxis::PCA };
	std::unique_ptr<BVHNode> m_RootNode{};
};