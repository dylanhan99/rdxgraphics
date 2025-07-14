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
		if (other.Left) Left = std::make_unique<BVHNode>(*other.Left);
		if (other.Right) Right = std::make_unique<BVHNode>(*other.Right);
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

struct BVHNode_Mult
{
	inline BVHNode_Mult() = default;
	inline BVHNode_Mult(BVHNode_Mult const& other)
	{
		*this = other;
	}
	inline BVHNode_Mult& operator=(BVHNode_Mult const& other)
	{
		Handle = other.Handle;
		for (size_t i = 0; i < other.Children.size(); ++i)
			Children.emplace_back(std::move(std::make_unique<BVHNode_Mult>(*other.Children[i])));
		Objects = other.Objects;
		return *this;
	}

	struct TypeNode : public BaseComponent { char _{}; };
	struct TypeLeaf : public BaseComponent { char _{}; };

	entt::entity Handle{ entt::null }; // Handle to this node's bounding bolume
	std::vector<std::unique_ptr<BVHNode_Mult>> Children{};
	std::vector<entt::entity> Objects{};

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
		BottomUp,
		OctTree,
		KDTree
	};
	enum class LeafCondition {
		OneEntity,
		TwoEntitiesMax,
		TreeHeightTwo
	};
	enum class SplitPointStrat {
		MedianCenters,
		MedianExtents,
		KEvenSplits
	};
	enum class MergeStrat {
		NearestNeighbour,
		MinVolume,
		MinSurfaceArea
	};
	enum class StraddleCondition {
		ObjectCenter,
		AllOverlapping,
		CurrentLevel,
		SplitObject
	};

private:
	// Topdown stuff
	using Entity = entt::entity;
	using EntityList = std::vector<Entity>;

	// Bottomup stuff
	using NodeList = std::vector<BVHNode>;
	using HeuristicCache = std::map<entt::entity, std::map<entt::entity, float>>;

	// OctTree stuff


public:
	static bool Init();
	static void EnforceUniformBVs();

	inline static std::unique_ptr<BVHNode>& GetRootNode() { return g.m_RootNode; }
	inline static std::unique_ptr<BVHNode_Mult>& GetRootNode_Mult() { return g.m_RootNode_Mult; }
	inline static BV& GetGlobalBVType() { return g.m_GlobalBVType; }
	inline static BVHType& GetCurrentTreeType() { return g.m_CurrentTreeType; }
	inline static int& GetDrawLayers() { return g.m_DrawLayers; }
	inline static int& GetBVHHeight() { return g.m_BVHHeight; }
	inline static int& GetObjsPerNode() { return g.m_ObjsPerNode; }
	inline static int& GetMaxHeight() { return g.m_MaxHeight; }

	// Topdown stuff
	inline static LeafCondition& GetCurrentLeafCondition() { return g.m_CurrentLeafCondition; }
	inline static SplitPointStrat& GetCurrentSplitPointStrat() { return g.m_CurrentSplitPointStrat; }
	// Bottomup stuff
	inline static MergeStrat& GetCurrentMergeStrat() { return g.m_CurrentMergeStrat; }
	// OctTree stuff
	inline static StraddleCondition& GetCurrentStraddleCondition() { return g.m_CurrentStraddleCondition; }

	// *** Helper functions *** //
	static void BuildBVH();
	static void DestroyBVH(std::unique_ptr<BVHNode>& pNode);
	static void DestroyBVH(std::unique_ptr<BVHNode_Mult>& pNode);

	template <typename T>
	static T ComputeBV(Entity*, int);
	template <typename T>
	static float HeuristicCost(T const& bvL, int const numL, T const& bvR, int const numR, T const& bvTotal);
	static int FindDominantAxis(Entity* entities, int numEnts, AABBBV* pBV = nullptr);
	static int Partition(Entity* pEntities, int numEnts, float const splitPoint);
	static int Heuristic_MedianCenters(Entity* pEntities, int numEnts);
	static int Heuristic_MedianExtents(Entity* pEntities, int numEnts, int axis, AABBBV const& totalBV);
	static int Heuristic_KEvenSplits(Entity* pEntities, int numEnts, int axis, float splitPoint);
	static int Heuristic_SmallestSFA(Entity* pEntities, int numEnts);

	static void FindNodesToMerge(NodeList& nodeList, NodeList::const_iterator& itFirst, NodeList::const_iterator& itSecond, HeuristicCache const& cache);
	static void UpdateHeuristicCache(BVHNode const& newNode, HeuristicCache& cache);
	template <typename T>
	static float CalculateHeuristicCost(T const& bvL, T const& bvR);
	// *** *** //

	// *** Tree building *** //
	static void BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* pEntities, int numEnts, int height = 0, float kEvenStartPoint = 0.f, float kEvenWidth = 0.f);
	static void BVHTree_BottomUp(std::unique_ptr<BVHNode>& pNode, NodeList& nodeList, HeuristicCache& cache);
	static void BVHTree_OctTree(std::unique_ptr<BVHNode_Mult>& pNode, std::vector<Entity> const& entities, glm::vec3 const& nodePos, glm::vec3 const& nodeHalfE, int height = 0);
	// *** *** //

private:
	inline static int GetTopdownAxis() { return g.m_TopdownAxis; }

private:
	BV m_GlobalBVType{ BV::AABB };
	BVHType m_CurrentTreeType{ BVHType::OctTree };
	int m_DrawLayers{ INT_MAX }; // Enable all layers by default
	int m_BVHHeight{ 0 }; // Just a cache of the current BVH's height for rendering and stuff
	std::unique_ptr<BVHNode> m_RootNode{};
	std::unique_ptr<BVHNode_Mult> m_RootNode_Mult{};

	// TopDown stuff
	LeafCondition m_CurrentLeafCondition{ LeafCondition::OneEntity };
	SplitPointStrat m_CurrentSplitPointStrat{ SplitPointStrat::MedianCenters };
	int m_TopdownAxis{}; // Dumb hack to make things a little less messy. Only meant for K-Even Splits
	// BottomUp stuff
	MergeStrat m_CurrentMergeStrat{ MergeStrat::NearestNeighbour };

	// Oct and KD stuff
	int m_ObjsPerNode{ 10 };
	int m_MaxHeight{ 6 };
	// OctTree stuff
	StraddleCondition m_CurrentStraddleCondition{ StraddleCondition::AllOverlapping };
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

template <typename T>
static float BVHSystem::CalculateHeuristicCost(T const& bvL, T const& bvR)
{
	float cost{};
	switch (GetCurrentMergeStrat())
	{
	case MergeStrat::NearestNeighbour:
	{ 
		cost = glm::distance2(
			bvL.GetPosition(),
			bvR.GetPosition()
		);
		break; 
	}
	case MergeStrat::MinVolume:
	{ 
		T temp{};
		temp.RecalculateBV(bvL, bvR);
		cost = temp.GetVolume();
		break; 
	}
	case MergeStrat::MinSurfaceArea:
	{ 
		T temp{};
		temp.RecalculateBV(bvL, bvR);
		cost = temp.GetSurfaceArea();
		break;
	}
	default: break;
	}
	return cost;
}