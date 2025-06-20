#pragma once
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

struct BVHNode
{
	struct TypeNode : public BaseComponent { char _{}; };
	struct TypeLeaf : public BaseComponent { char _{}; };

	entt::entity Handle{ entt::null };
	std::unique_ptr<BVHNode> Left{nullptr};
	std::unique_ptr<BVHNode> Right{nullptr};

	void SetIsNode() const;
	void SetIsLeaf() const;
	bool IsLeaf() const;
};

class BVHSystem : public BaseSingleton<BVHSystem>
{
	RX_SINGLETON_DECLARATION(BVHSystem);
private:
	using Entity = std::pair<entt::entity, float>; // handle, dotproduct on axis
	using EntityList = std::vector<Entity>;

public:
	static bool Init();
	static void EnforceUniformBVs();
	// BVHTree_TopDown includes the in-place recalculation of BVH leafs
	static void BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* entities, size_t numEnts);
	static void DestroyBVH(std::unique_ptr<BVHNode>& pNode);
	static std::unique_ptr<BVHNode>& GetRoot() { return g.m_RootNode; }

	inline static BV& GetGlobalBVType() { return g.m_GlobalBVType; }

	static EntityList GetSortedEntities();
	static int PartitionEntities(Entity* entities, size_t numEnts);

private:

private:
	BV m_GlobalBVType{ BV::AABB };
	std::unique_ptr<BVHNode> m_RootNode{};
};