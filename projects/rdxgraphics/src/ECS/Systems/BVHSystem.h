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
	enum class LeafCondition {
		OneEntity,
		TwoEntitiesMax,
		TreeHeightTwo
	};
	enum class SplitPointStrat {
		MedianCenters,
		MedianExtents,
		KEvenSplits,
		SmallestSFA
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
	inline static LeafCondition& GetCurrentLeafCondition() { return g.m_CurrentLeafCondition; }
	inline static SplitPointStrat& GetCurrentSplitPointStrat() { return g.m_CurrentSplitPointStrat; }

	// *** Helper functions *** //
	static void BuildBVH();
	static void BuildBVH(std::function<void(std::unique_ptr<BVHNode>&, Entity*, int, int)> fnBuildBVH);
	static void DestroyBVH(std::unique_ptr<BVHNode>& pNode);

	template <typename T>
	static T ComputeBV(Entity*, int);
	template <typename T>
	static float HeuristicCost(T const& bvL, int const numL, T const& bvR, int const numR, T const& bvTotal);
	static int FindDominantAxis(Entity* entities, int numEnts, AABBBV* pBV = nullptr);
	static int Partition(Entity* pEntities, int numEnts);
	static int Heuristic_MedianCenters(Entity* pEntities, int numEnts);
	static int Heuristic_MedianExtents(Entity* pEntities, int numEnts, int axis, AABBBV const& totalBV);
	static int Heuristic_KEvenSplits(Entity* pEntities, int numEnts);
	static int Heuristic_SmallestSFA(Entity* pEntities, int numEnts);
	// *** *** //

	// *** Tree building *** //
	static void BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* pEntities, int numEnts, int height = 0);
	// *** *** //

private:

private:
	BV m_GlobalBVType{ BV::AABB };
	BVHType m_CurrentTreeType{ BVHType::TopDown };
	LeafCondition m_CurrentLeafCondition{ LeafCondition::OneEntity };
	SplitPointStrat m_CurrentSplitPointStrat{ SplitPointStrat::MedianCenters };
	std::unique_ptr<BVHNode> m_RootNode{};
};

template <typename T>
static T BVHSystem::ComputeBV(Entity* pEntities, int numEnts)
{
	T ret{};
	for (int i = 0; i < numEnts; ++i)
	{
		T& bv = EntityManager::GetComponent<T>(pEntities[i].first);
		if (i == 0)
		{
			ret.RecalculateBV(bv);
			continue;
		}

		ret.RecalculateBV(ret, bv);
	}

	return std::move(ret);
}

template <typename T>
static float BVHSystem::HeuristicCost(T const& bvL, int const numL, T const& bvR, int const numR, T const& bvTotal)
{
	float const invAll = 1.f / bvTotal.GetSurfaceArea();
	float const normL = bvL.GetSurfaceArea() * invAll;
	float const normR = bvR.GetSurfaceArea() * invAll;

	return numL * normL + numR * normR;
}