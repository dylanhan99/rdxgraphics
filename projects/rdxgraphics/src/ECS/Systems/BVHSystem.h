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

private:
	using Entity = std::pair<entt::entity, Xform&>;
	using EntityList = std::vector<Entity>;

public:
	static bool Init();
	static void EnforceUniformBVs();
	inline static std::unique_ptr<BVHNode>& GetRootNode() { return g.m_RootNode; }
	inline static BV& GetGlobalBVType() { return g.m_GlobalBVType; }
	inline static BVHType& GetCurrentTreeType() { return g.m_CurrentTreeType; }

	// *** Helper functions *** //
	static void BuildBVH();
	static void BuildBVH(std::function<void(std::unique_ptr<BVHNode>&, Entity*, int, int)> fnBuildBVH);
	static void DestroyBVH(std::unique_ptr<BVHNode>& pNode);

	// Determines split plane
	template <typename T>
	static T ComputeBV(Entity*, int) { static_assert(false); return T{}; }
	template <typename T>
	static float HeuristicCost(T const& bvL, int numL, T const& bvR, int numR);
	static int FindDominantAxis(Entity* entities, int numEnts);
	static int Partition(Entity* pEntities, int numEnts);
	// *** *** //

	// *** Tree building *** //
	static void BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* pEntities, int numEnts, int height = 0);
	// *** *** //

private:

private:
	BV m_GlobalBVType{ BV::AABB };
	BVHType m_CurrentTreeType{ BVHType::TopDown };
	std::unique_ptr<BVHNode> m_RootNode{};
};

template <typename T>
static float BVHSystem::HeuristicCost(T const& bvL, int numL, T const& bvR, int numR)
{
	//bvL.GetVolume();
	//bvR.GetVolume();
	return 0.f;
}