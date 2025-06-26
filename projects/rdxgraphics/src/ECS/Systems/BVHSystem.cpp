#include "BVHSystem.h"
#include "Utils/IntersectionTests.h"

RX_SINGLETON_EXPLICIT(BVHSystem);

void BVHNode::SetIsNode() const
{
	if (!EntityManager::HasEntity(Handle))
		return;

	EntityManager::AddComponent<TypeNode>(Handle);
	EntityManager::RemoveComponent<TypeLeaf>(Handle);
}

void BVHNode::SetIsLeaf() const
{
	if (!EntityManager::HasEntity(Handle))
		return;

	EntityManager::RemoveComponent<TypeNode>(Handle);
	EntityManager::AddComponent<TypeLeaf>(Handle);
}

bool BVHNode::IsLeaf() const
{
	return EntityManager::HasComponent<TypeLeaf>(Handle);
}

bool BVHSystem::Init()
{
	//EnforceUniformBVs();
	//auto sortedEntities = GetSortedEntities();
	//BVHTree_TopDown(g.m_RootNode, sortedEntities.data(), sortedEntities.size());

	return true;
}

void BVHSystem::EnforceUniformBVs()
{
	// Dirty BV handling needs to be updated to ensure proper 
	// calculation between NODES and LEAFS
	// Current idea: to calculate all leafs first, then work back up.
	// Can do this recursively via root node, then MakeBVH(lhs, rhs) which just combines the two.

	auto view = EntityManager::View<BoundingVolume>(entt::exclude<FrustumBV>);
	for (auto [handle, bv] : view.each())
		bv.SetBVType(BVHSystem::GetGlobalBVType());
}

void BVHSystem::BuildBVH()
{
	DestroyBVH(GetRootNode());

	EntityList entities{};
	{
		auto view = EntityManager::View<Xform, BoundingVolume const>(entt::exclude<FrustumBV>);
		for (auto [handle, xform, __] : view.each())
			entities.emplace_back(Entity{ handle, xform });
	}

	switch (GetCurrentTreeType())
	{
	case BVHType::TopDown:
	{
		BVHTree_TopDown(GetRootNode(), entities.data(), (int)entities.size(), 0);
		break;
	}
	case BVHType::BottomUp:
	{
		g.m_RootNode = BVHTree_BottomUp(entities.data(), (int)entities.size());
		break;
	}
	default: break;
	}
}

void BVHSystem::DestroyBVH(std::unique_ptr<BVHNode>& pNode)
{
	if (!pNode)
		return;

	EntityManager::Destroy(pNode->Handle);
	DestroyBVH(pNode->Left);
	DestroyBVH(pNode->Right);
	pNode->Objects.clear();
	pNode.reset(nullptr);
}

int BVHSystem::FindDominantAxis(Entity* entities, int numEnts, AABBBV* pBV)
{
	// Find the dominant plane, return 0,1,2 corresponding to x,y,z
	glm::vec3 aabbExtents{};
	{
		glm::vec3 min{ std::numeric_limits<float>().infinity() };
		glm::vec3 max{ -std::numeric_limits<float>().infinity() };

		for (int i = 0; i < numEnts; ++i)
		{
			Xform& xform = entities[i].second;
			glm::vec3 const& pos = xform.GetTranslate();

			min.x = glm::min(min.x, pos.x);
			min.y = glm::min(min.y, pos.y);
			min.z = glm::min(min.z, pos.z);

			max.x = glm::max(max.x, pos.x);
			max.y = glm::max(max.y, pos.y);
			max.z = glm::max(max.z, pos.z);
		}

		aabbExtents = (min - max);
		if (pBV)
		{
			pBV->GetHalfExtents() = aabbExtents * 0.5f;
			pBV->SetPosition((min + max) * 0.5f);
		}
	}

	if (aabbExtents.x > aabbExtents.y && aabbExtents.x > aabbExtents.z)
		return 0; // X dom
	if (aabbExtents.y > aabbExtents.z)
		return 1; // Y dom
	return 2; // Z dom
}

int BVHSystem::Partition(Entity* pEntities, int numEnts)
{
	if (!pEntities || numEnts <= 0)
		return 0;

	AABBBV totalBV{}; // This simply caches the info of encompassing BV, be it Sphere or AABB
	int axis = FindDominantAxis(pEntities, numEnts, &totalBV);
	auto span = std::span(pEntities, pEntities + numEnts);
	std::sort(span.begin(), span.end(),
		[&axis](Entity const& lhs, Entity const& rhs)
		{
			glm::vec3 const& l = lhs.second.GetTranslate();
			glm::vec3 const& r = rhs.second.GetTranslate();

			return l[axis] < r[axis];
		});

	int k{};
	switch (GetCurrentSplitPointStrat())
	{
	case SplitPointStrat::MedianCenters:
		k = Heuristic_MedianCenters(pEntities, numEnts);
		break;
	case SplitPointStrat::MedianExtents:
		k = Heuristic_MedianExtents(pEntities, numEnts, axis, totalBV);
		break;
	case SplitPointStrat::KEvenSplits:
		k = Heuristic_KEvenSplits(pEntities, numEnts);
		break;
	case SplitPointStrat::SmallestSFA:
		k = Heuristic_SmallestSFA(pEntities, numEnts);
		break;
	default: break;
	}
	return k;
}

int BVHSystem::Heuristic_MedianCenters(Entity* pEntities, int numEnts)
{
	return numEnts / 2;
}

int BVHSystem::Heuristic_MedianExtents(Entity* pEntities, int numEnts, int axis, AABBBV const& totalBV)
{
	float splitPoint = totalBV.GetPosition()[axis]; // Median
	for (int k = 0; k < numEnts; ++k)
	{
		if (pEntities[k].second.GetTranslate()[axis] > splitPoint)
			return k;
	}

	return Heuristic_MedianCenters(pEntities, numEnts);
}

int BVHSystem::Heuristic_KEvenSplits(Entity* pEntities, int numEnts)
{
	return 1;
}

int BVHSystem::Heuristic_SmallestSFA(Entity* pEntities, int numEnts)
{
	float minCost{ std::numeric_limits<float>().infinity() };
	int k = 1;
#define _RX_X(Klass)														\
	case BV::Klass: {														\
		Klass##BV bvTotal = ComputeBV<Klass##BV>(pEntities, numEnts);		\
		for (int i = k; i < numEnts; ++i)									\
		{																	\
			Klass##BV bvL = ComputeBV<Klass##BV>(pEntities, i);				\
			Klass##BV bvR = ComputeBV<Klass##BV>(pEntities, numEnts - i);	\
			float cost = HeuristicCost(bvL, i, bvR, numEnts - i, bvTotal);	\
			if (cost < minCost)												\
			{																\
				minCost = cost;												\
				k = i;														\
			}																\
		}																	\
	} break;
	switch (GetGlobalBVType())
	{
		RX_DO_ALL_BVH_ENUM_M(_RX_X);
	default: RX_ASSERT(false); break;
	}

	return k;
}

void BVHSystem::BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* pEntities, int numEnts, int height)
{
	if (numEnts <= 0 || !pEntities)
	{
		pNode.reset(nullptr);
		return;
	}

	pNode = std::make_unique<BVHNode>();
	entt::entity const& handle = EntityManager::CreateEntity<Xform>();
	pNode->Handle = handle;
	auto& objs = pNode->Objects;
	{ // Objects. This is kinda bad cus it's so much extra copied memory
		objs.clear();
		objs.resize(numEnts);
		for (int i = 0; i < numEnts; ++i)
			objs[i] = pEntities[i].first; // Crappy copy
	}

	// We can assume Objects is not empty.
	constexpr int MAX_OBJS_PER_LEAF = 2;
	constexpr int MAX_TREE_HEIGHT = 2;

	bool leafCondition = numEnts == 1; // Default condition
	switch (GetCurrentLeafCondition())
	{
	case LeafCondition::OneEntity:
		//leafCondition = numEnts == 1;
		break;
	case LeafCondition::TwoEntitiesMax:
		leafCondition |= numEnts <= MAX_OBJS_PER_LEAF;
		break;
	case LeafCondition::TreeHeightTwo:
		leafCondition |= height >= MAX_TREE_HEIGHT;
		break;
	default: break;
	}

	if (leafCondition)
	{ // At an actual game object, we can simply use the entt handle and set LEAF
		// Some function to combine the sizes of the numEnts number of pEntities
		pNode->SetIsLeaf();
#define _RX_X(Klass)																\
		case BV::Klass:																\
		{																			\
			EntityManager::AddComponent<BoundingVolume>(handle, BV::Klass);			\
			Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(handle);			\
			{																		\
				Klass##BV& other = EntityManager::GetComponent<Klass##BV>(objs[0]);	\
				bv.RecalculateBV(other);											\
			}																		\
			if (pNode->Objects.size() > 1)											\
			{																		\
				for (size_t i = 0; i < objs.size(); ++i)							\
				{																	\
					entt::entity const& ent = objs[i];								\
					Klass##BV& other = EntityManager::GetComponent<Klass##BV>(ent);	\
					bv.RecalculateBV(bv, other);									\
				}																	\
			}																		\
			break;																	\
		}
		switch (GetGlobalBVType())
		{
			RX_DO_ALL_BVH_ENUM_M(_RX_X);
		default: break;
		}
#undef _RX_X

		//entt::entity const& handle = pEntities[0].first;
		//pNode->Handle = handle;
		//pNode->SetIsLeaf();
	}
	else
	{ // Is a node,
		// Partition and recurse, then combine the children BVs to build your own
		// Set to NODE
		pNode->SetIsNode();

		int k = Partition(pEntities, numEnts);
		Entity* pEntitiesL = pEntities;
		Entity* pEntitiesR = pEntities + k;

		++height;
		BVHTree_TopDown(pNode->Left, pEntitiesL, k, height);
		BVHTree_TopDown(pNode->Right, pEntitiesR, numEnts - k, height);

		// Now we can merge the BVs
		// hardcode assuming AABB for now
#define _RX_X(Klass)																	  \
		case BV::Klass:																	  \
		{																				  \
			EntityManager::AddComponent<BoundingVolume>(handle, BV::Klass);				  \
			Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(handle);				  \
			Klass##BV& bvL = EntityManager::GetComponent<Klass##BV>(pNode->Left->Handle); \
			Klass##BV& bvR = EntityManager::GetComponent<Klass##BV>(pNode->Right->Handle);\
			bv.RecalculateBV(bvL, bvR);													  \
			break;																		  \
		}
		switch (GetGlobalBVType())
		{
			RX_DO_ALL_BVH_ENUM_M(_RX_X);
		default: break;
		}
#undef _RX_X
	}

	// We still need the Xform to be updated, so leave it be. But BV is already 
	// calculated using RecalculateBV, so just force remove DirtyBV component
	{
		//EntityManager::RemoveComponent<BoundingVolume::DirtyXform>(handle);
		EntityManager::RemoveComponent<BoundingVolume::DirtyBV>(handle);
	}
}

std::unique_ptr<BVHNode> BVHSystem::BVHTree_BottomUp(Entity* pEntities, int numEnts)
{
	return std::unique_ptr<BVHNode>();
}
