#pragma once
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

struct BVHNode
{
	inline BVHNode() = default;
	inline BVHNode(BVHNode const& other)
	{
		*this = other;
	}
	inline BVHNode& operator=(BVHNode const& other)
	{
		Handle = other.Handle;
		Left = std::make_unique<BVHNode>(*other.Left);
		Right = std::make_unique<BVHNode>(*other.Right);
		Objects = other.Objects;
		return *this;
	}

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
		BottomUp
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
	using Entity = entt::entity;
	using EntityList = std::vector<Entity>;

public:
	static bool Init();
	static void EnforceUniformBVs();
	inline static std::unique_ptr<BVHNode>& GetRootNode() { return g.m_RootNode; }
	inline static BV& GetGlobalBVType() { return g.m_GlobalBVType; }
	inline static BVHType& GetCurrentTreeType() { return g.m_CurrentTreeType; }
	inline static LeafCondition& GetCurrentLeafCondition() { return g.m_CurrentLeafCondition; }
	inline static SplitPointStrat& GetCurrentSplitPointStrat() { return g.m_CurrentSplitPointStrat; }
	inline static int& GetDrawLayers() { return g.m_DrawLayers; }
	inline static int& GetBVHHeight() { return g.m_BVHHeight; }

	// *** Helper functions *** //
	static void BuildBVH();
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
	static void BVHTree_BottomUp(std::unique_ptr<BVHNode>& pNode, std::vector<BVHNode>& nodeList);
	// *** *** //

private:

private:
	BV m_GlobalBVType{ BV::AABB };
	BVHType m_CurrentTreeType{ BVHType::TopDown };
	LeafCondition m_CurrentLeafCondition{ LeafCondition::OneEntity };
	SplitPointStrat m_CurrentSplitPointStrat{ SplitPointStrat::MedianCenters };
	std::unique_ptr<BVHNode> m_RootNode{};

	int m_DrawLayers{ INT_MAX }; // Enable all layers by default
	int m_BVHHeight{ 0 }; // Just a cache of the current BVH's height for rendering and stuff
};

template <typename T>
static T BVHSystem::ComputeBV(Entity* pEntities, int numEnts)
{
	T ret{};
	for (int i = 0; i < numEnts; ++i)
	{
		T& bv = EntityManager::GetComponent<T>(pEntities[i]);
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