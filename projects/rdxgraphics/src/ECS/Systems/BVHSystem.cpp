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

void BVHNode_Mult::SetIsNode() const
{
	if (!EntityManager::HasEntity(Handle))
		return;

	EntityManager::AddComponent<TypeNode>(Handle);
	EntityManager::RemoveComponent<TypeLeaf>(Handle);
}

void BVHNode_Mult::SetIsLeaf() const
{
	if (!EntityManager::HasEntity(Handle))
		return;

	EntityManager::RemoveComponent<TypeNode>(Handle);
	EntityManager::AddComponent<TypeLeaf>(Handle);
}

bool BVHNode_Mult::IsLeaf() const
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
	DestroyBVH(GetRootNode_Mult());
	g.m_BVHHeight = -std::numeric_limits<int>().infinity();

	switch (GetCurrentTreeType())
	{
	case BVHType::TopDown:
	{
		EntityList entities{};
		{
			auto view = EntityManager::View<Xform, BoundingVolume const>(entt::exclude<FrustumBV>);
			for (auto [handle, _, __] : view.each())
				entities.emplace_back(Entity{ handle });
		}

		float kEvenStartPoint{}, kEvenWidth{};
		if (SplitPointStrat::KEvenSplits == GetCurrentSplitPointStrat())
		{
			AABBBV totalBV{};
			int axis = FindDominantAxis(entities.data(), entities.size(), &totalBV);
			g.m_TopdownAxis = axis;

			kEvenStartPoint = totalBV.GetMinPoint()[axis];
			kEvenWidth = glm::abs(totalBV.GetMaxPoint()[axis] - kEvenStartPoint);
		}

		BVHTree_TopDown(GetRootNode(), entities.data(), (int)entities.size(), 0,
			kEvenStartPoint, kEvenWidth);
		break;
	}
	case BVHType::BottomUp:
	{
		std::vector<BVHNode> nodeList{};
		HeuristicCache cache{};
		{
			auto view = EntityManager::View<Xform, BoundingVolume const>(entt::exclude<FrustumBV>);
			for (auto [handle, _, __] : view.each())
			{
				BVHNode newNode{};
				entt::entity const nodeHandle = newNode.Handle = EntityManager::CreateEntity<Xform>();
				newNode.Objects.push_back(handle);
				newNode.SetIsLeaf();
				EntityManager::AddComponent<BoundingVolume>(nodeHandle, GetGlobalBVType());
#define _RX_X(Klass)																		\
				case BV::Klass:																\
				{																			\
					Klass##BV& originalBV = EntityManager::GetComponent<Klass##BV>(handle);	\
					Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(nodeHandle);		\
					bv.RecalculateBV(originalBV);											\
					break;																	\
				}

				switch (GetGlobalBVType())
				{
					RX_DO_ALL_BVH_ENUM_M(_RX_X);
				default: break;
				}
#undef _RX_X
				EntityManager::RemoveComponent<BoundingVolume::DirtyBV>(nodeHandle);
				nodeList.emplace_back(std::move(newNode));
				cache[nodeHandle]; // Default init
			}
		}

		{
			for (BVHNode const& node : nodeList)
			{
				UpdateHeuristicCache(node, cache);
			}
		}

		BVHTree_BottomUp(GetRootNode(), nodeList, cache);
		break;
	}
	case BVHType::OctTree:
	{
		EntityList entities{};
		{
			auto view = EntityManager::View<Xform, BoundingVolume const>(entt::exclude<FrustumBV>);
			for (auto [handle, _, __] : view.each())
				entities.emplace_back(Entity{ handle });
		}

		AABBBV totalBV = ComputeBV<AABBBV>(entities.data(), entities.size());
		BVHTree_OctTree(GetRootNode_Mult(), entities, totalBV.GetPosition(), totalBV.GetHalfExtents());
		break;
	}
	case BVHType::KDTree:
	{
		std::vector<std::pair<Entity, glm::vec3>> points{};
		EntityList entities{};
		{
			auto view = EntityManager::View<Xform, BoundingVolume const>(entt::exclude<FrustumBV>);
			for (auto [handle, _, __] : view.each())
			{
				entities.emplace_back(Entity{ handle });

				AABBBV& bv = EntityManager::GetComponent<AABBBV>(handle);
				switch (GetCurrentSplitPointStrat())
				{
				case SplitPointStrat::MedianExtents:
				{
					points.emplace_back(std::make_pair(handle, bv.GetMinPoint()));
					points.emplace_back(std::make_pair(handle, bv.GetMaxPoint()));
					break;
				}
				case SplitPointStrat::MedianCenters:
				default:
				{
					points.emplace_back(std::make_pair(handle, bv.GetPosition()));
					break;
				}
				}
			}
		}

		AABBBV totalBV = ComputeBV<AABBBV>(entities.data(), entities.size());
		BVHTree_KDTree(GetRootNode(), points, 0, totalBV.GetPosition(), totalBV.GetHalfExtents());
		break;
	}
	default: break;
	}
}

void BVHSystem::DestroyBVH(std::unique_ptr<BVHNode>& pNode)
{
	if (!pNode || !EntityManager::HasEntity(pNode->Handle))
		return;

	EntityManager::Destroy(pNode->Handle);
	DestroyBVH(pNode->Left);
	DestroyBVH(pNode->Right);
	pNode->Objects.clear();
	pNode.reset(nullptr);
}

void BVHSystem::DestroyBVH(std::unique_ptr<BVHNode_Mult>& pNode)
{
	if (!pNode || !EntityManager::HasEntity(pNode->Handle))
		return;

	EntityManager::Destroy(pNode->Handle);
	for (auto& pChild : pNode->Children)
		DestroyBVH(pChild);
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
			Xform const& xform = EntityManager::GetComponent<Xform const>(entities[i]);
			glm::vec3 const& pos = xform.GetTranslate();

			min.x = glm::min(min.x, pos.x);
			min.y = glm::min(min.y, pos.y);
			min.z = glm::min(min.z, pos.z);

			max.x = glm::max(max.x, pos.x);
			max.y = glm::max(max.y, pos.y);
			max.z = glm::max(max.z, pos.z);
		}

		aabbExtents = glm::abs(min - max);
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

int BVHSystem::Partition(Entity* pEntities, int numEnts, float const splitPoint)
{
	if (!pEntities || numEnts <= 0)
		return 0;

	AABBBV totalBV{}; // This simply caches the info of encompassing BV, be it Sphere or AABB
	int axis = SplitPointStrat::KEvenSplits == GetCurrentSplitPointStrat() ?
		GetTopdownAxis() : 
		FindDominantAxis(pEntities, numEnts, &totalBV);

	auto span = std::span(pEntities, pEntities + numEnts);
#define _RX_X(Klass)												     \
	case BV::Klass:													     \
	{																     \
		Klass##BV const& l = EntityManager::GetComponent<Klass##BV>(lhs);\
		Klass##BV const& r = EntityManager::GetComponent<Klass##BV>(rhs);\
		return l.GetPosition()[axis] < r.GetPosition()[axis];			 \
	}
	std::sort(span.begin(), span.end(),
		[axis](Entity const& lhs, Entity const& rhs)
		{
			switch (GetGlobalBVType())
			{
				RX_DO_ALL_BVH_ENUM_M(_RX_X);
			default: return false;
			}
		});
#undef _RX_X

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
		k = Heuristic_KEvenSplits(pEntities, numEnts, axis, splitPoint);
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
		Xform const& xform = EntityManager::GetComponent<Xform const>(pEntities[k]);
		if (xform.GetTranslate()[axis] > splitPoint)
			return k;
	}

	return Heuristic_MedianCenters(pEntities, numEnts);
}

int BVHSystem::Heuristic_KEvenSplits(Entity* pEntities, int numEnts, int axis, float splitPoint)
{
	int k = 0;
	float min = std::numeric_limits<float>().infinity();
	float max = -std::numeric_limits<float>().infinity();

	for (; k < numEnts; ++k)
	{
		Xform const& xform = EntityManager::GetComponent<Xform const>(pEntities[k]);
		float const& val = xform.GetTranslate()[axis];
		min = glm::min(min, val);
		max = glm::max(max, val);
		if (val > splitPoint)
			break;
	}

	bool isDegenerateSplit = glm::abs(max - min) < glm::epsilon<float>();
	if (isDegenerateSplit)
		k = Heuristic_MedianCenters(pEntities, numEnts);

	return k;
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
#undef _RX_X

	return k;
}

void BVHSystem::FindNodesToMerge(NodeList& nodeList, NodeList::const_iterator& itFirst, NodeList::const_iterator& itSecond, HeuristicCache const& cache)
{
	//itFirst = nodeList.begin();
	//itSecond = nodeList.begin();
	//std::advance(itSecond, 1);
	//return;

	// Cache of entts and the heuristic against other entts
	auto& heuristicCache = cache;

	// Find the nearest neighbour
	float minHeuristic{ std::numeric_limits<float>().infinity() }; // Current pseudocode example, this is distance
	std::pair<entt::entity, entt::entity> bestPair{ entt::null, entt::null };
	for (int i = 0; i < nodeList.size(); ++i)
	{
		BVHNode const& lhs = nodeList[i];
		auto const& lhsCostMap = heuristicCache.at(lhs.Handle);
		for (int j = 0; j < nodeList.size(); ++j)
		{
			if (i == j) continue;
			BVHNode const& rhs = nodeList[j];
			float const& rhsCost = lhsCostMap.at(rhs.Handle);

			// Now, we can finally perform the heuristic check to see if it is lowest or not

			if (rhsCost > minHeuristic) // Skip if comparison not even met
				continue;
	
			if (rhsCost == minHeuristic) // It's equal, introduce your secondary heuristic
				continue; // Skip for now
	
			// Now, we definitely want to replace the min with the new found min
			minHeuristic = rhsCost;
			bestPair = std::make_pair(lhs.Handle, rhs.Handle);
		}
	}

	// Returning the results
	itFirst = std::find_if(nodeList.begin(), nodeList.end(),
		[h = bestPair.first](BVHNode const& node)
		{
			return node.Handle == h;
		});
	itSecond = std::find_if(nodeList.begin(), nodeList.end(),
		[h = bestPair.second](BVHNode const& node)
		{
			return node.Handle == h;
		});

	// If you hit either of these, it means your node is for some reason not even inserted
	// into nodeList to begin with
	RX_ASSERT(itFirst != nodeList.end());
	RX_ASSERT(itSecond != nodeList.end());
}

void BVHSystem::UpdateHeuristicCache(BVHNode const& newNode, HeuristicCache& cache)
{
	// Craeting the map if not already there
	auto& mapL = cache[newNode.Handle];
	// Updating all existing keys
	for (auto& [key, mapR] : cache)
	{
		float cost{};
#define _RX_X(Klass)																\
		case BV::Klass:																\
		{																			\
			Klass##BV& bvL = EntityManager::GetComponent<Klass##BV>(key);			\
			Klass##BV& bvR = EntityManager::GetComponent<Klass##BV>(newNode.Handle);\
			cost = CalculateHeuristicCost(bvL, bvR);								\
			break;																	\
		}
		switch (GetGlobalBVType())
		{
			RX_DO_ALL_BVH_ENUM_M(_RX_X);
		default: break;
		}
#undef _RX_X

		mapL.emplace(key, cost);
		mapR.emplace(newNode.Handle, cost);
	}
}

void BVHSystem::BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* pEntities, int numEnts, int height, float kEvenStartPoint, float kEvenWidth)
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
			objs[i] = pEntities[i]; // Crappy copy
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
			for (size_t i = 0; i < objs.size(); ++i)								\
			{																		\
				entt::entity const& ent = objs[i];									\
				Klass##BV& other = EntityManager::GetComponent<Klass##BV>(ent);		\
				if (i == 0)															\
				{																	\
					bv.RecalculateBV(other);										\
					continue;														\
				}																	\
				bv.RecalculateBV(bv, other);										\
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

		// updating height
		if (height > g.m_BVHHeight)
			g.m_BVHHeight = height;
	}
	else
	{ // Is a node,
		// Partition and recurse, then combine the children BVs to build your own
		// Set to NODE
		pNode->SetIsNode();

		float kEvenSplitPoint = kEvenStartPoint + kEvenWidth * 0.5f;
		int k = Partition(pEntities, numEnts, kEvenSplitPoint);
		Entity* pEntitiesL = pEntities;
		Entity* pEntitiesR = pEntities + k;

		if (SplitPointStrat::KEvenSplits == GetCurrentSplitPointStrat())
		{ // K-even splits needs special attention to properly manage split point and width
			// case 1: K == 0 which means ALL are on the RIGHT of split point
			// case 2: K == numEnts which means ALL are on the LEFT of split point
			bool case1{ false }, case2{ false };
			float newStartPoint = kEvenStartPoint;
			float newWidth = kEvenWidth;
			while (true)
			{
				newWidth *= 0.5f;
				case1 = k <= 0;
				case2 = k >= numEnts;
				if (!case1 && !case2)
					break;


				float newSplitPoint{};
				if (case1)
				{
					// Shift the start point to the right
					newStartPoint += newWidth;
				}
				else if (case2)
				{
					// Start point remains
					//newStartPoint;
				}
				newSplitPoint = newStartPoint + newWidth * 0.5f;
				k = Partition(pEntitiesL, numEnts, newSplitPoint);
			}

			// standard case, a proper split, handle as we do normally
			pEntitiesL = pEntities;
			pEntitiesR = pEntities + k;

			// Set the next start point and width
			kEvenStartPoint = newStartPoint;
			kEvenWidth = newWidth;
		}

		++height;
		BVHTree_TopDown(pNode->Left, pEntitiesL, k, height, kEvenStartPoint, kEvenWidth);
		BVHTree_TopDown(pNode->Right, pEntitiesR, numEnts - k, height, kEvenStartPoint + kEvenWidth, kEvenWidth);

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

void BVHSystem::BVHTree_BottomUp(std::unique_ptr<BVHNode>& pNode, NodeList& nodeList, HeuristicCache& cache)
{
	if (nodeList.empty())
	{
		pNode.reset(nullptr);
		return;
	}

	while (nodeList.size() != 1)
	{
		NodeList::const_iterator itFirst = nodeList.end();
		NodeList::const_iterator itSecond = nodeList.end();

		FindNodesToMerge(nodeList, itFirst, itSecond, cache);

		BVHNode newNode{};
		{ // Node data
			newNode.Handle = EntityManager::CreateEntity<Xform>();
			newNode.Left = std::make_unique<BVHNode>(*itFirst);
			newNode.Right = std::make_unique<BVHNode>(*itSecond);
			newNode.Objects.insert(newNode.Objects.end(), itFirst->Objects.begin(), itFirst->Objects.end());
			newNode.Objects.insert(newNode.Objects.end(), itSecond->Objects.begin(), itSecond->Objects.end());
			newNode.SetIsNode();
		}
		{ // Node's BV data
			// Merge the left and right bvs
			BoundingVolume& boundingVolume = EntityManager::AddComponent<BoundingVolume>(newNode.Handle, GetGlobalBVType());
#define _RX_X(Klass)																		   \
			case BV::Klass:																	   \
			{																				   \
				Klass##BV& bvL = EntityManager::GetComponent<Klass##BV>(newNode.Left->Handle); \
				Klass##BV& bvR = EntityManager::GetComponent<Klass##BV>(newNode.Right->Handle);\
				EntityManager::GetComponent<Klass##BV>(newNode.Handle).RecalculateBV(bvL, bvR);\
				break;																		   \
			}

			switch (boundingVolume.GetBVType())
			{
				RX_DO_ALL_BVH_ENUM_M(_RX_X);
			default: break;
			}
#undef _RX_X
		}

		if (itFirst < itSecond)
		{
			nodeList.erase(itSecond);
			nodeList.erase(itFirst);
		}
		else
		{
			nodeList.erase(itFirst);
			nodeList.erase(itSecond);
		}
		UpdateHeuristicCache(newNode, cache);
		nodeList.emplace_back(std::move(newNode));
	}

	pNode = std::make_unique<BVHNode>(std::move(nodeList[0]));
	if (pNode) // Determine height
	{
		std::function<int(std::unique_ptr<BVHNode> const&, int)> DetermineHeight{};
		DetermineHeight = 
			[&DetermineHeight](std::unique_ptr<BVHNode> const& ppNode, int height)->int
			{
				if (ppNode->IsLeaf())
					return height;

				height += 1;
				int left = DetermineHeight(ppNode->Left, height);
				int right = DetermineHeight(ppNode->Right, height);
				return glm::max(left, right);
			};

		int height = DetermineHeight(pNode, 0);
		g.m_BVHHeight = height;
	}
}

void BVHSystem::BVHTree_OctTree(std::unique_ptr<BVHNode_Mult>& pNode, std::vector<Entity> const& entities, glm::vec3 const& nodePos, glm::vec3 const& nodeHalfE, int height)
{
	pNode = std::make_unique<BVHNode_Mult>();
	if (entities.empty())
		return;

	{ // Node data
		pNode->Handle = EntityManager::CreateEntity<Xform>();
		pNode->Objects = entities;
		pNode->SetIsNode();	
	}
	{ // Node's BV data
		BoundingVolume& boundingVolume = EntityManager::AddComponent<BoundingVolume>(pNode->Handle, GetGlobalBVType());
		switch (boundingVolume.GetBVType())
		{
		case BV::AABB:
		{
			AABBBV& bv = EntityManager::GetComponent<AABBBV>(pNode->Handle);
			bv.SetPosition(nodePos);
			bv.GetHalfExtents() = nodeHalfE;
			break;
		}
		case BV::Sphere:
		{
			SphereBV& bv = EntityManager::GetComponent<SphereBV>(pNode->Handle);
			bv.SetPosition(nodePos);
			bv.GetRadius() = glm::compMax(nodeHalfE);
			break;
		}
		default: break;
		}
	}

	// Termination condition. Set this to a leaf.
	if (entities.size() <= GetObjsPerNode() || height >= GetMaxHeight())
	{
		pNode->SetIsLeaf();
		return;
	}

	glm::vec3 newHalfE = nodeHalfE * 0.5f;
	pNode->Children.resize(8);
	height += 1;
	// Handling each octant
	for (int i = 0; i < 8; ++i)
	{
		glm::vec3 dirNeg{
			(i & 1) ? 1.f : -1.f,
			(i & 2) ? 1.f : -1.f,
			(i & 4) ? 1.f : -1.f
		};
		glm::vec3 newNodePos = nodePos + (dirNeg * newHalfE);

		AABBBV temp{};
		temp.SetPosition(newNodePos);
		temp.GetHalfExtents() = newHalfE;

		std::vector<Entity> associatedEnts{};
		// Just use association to all for now. Easiest.
		for (Entity const& ent : entities)
		{
			switch (GetCurrentStraddleCondition())
			{
				case StraddleCondition::ObjectCenter:
				{
					AABBBV& objBV = EntityManager::GetComponent<AABBBV>(ent);
					// Check if directional vector from 
					//  parent node center to current node center is same alignment as [i]
					glm::vec3 v = objBV.GetPosition() - nodePos;
					bool isDir =
						(v.x * dirNeg.x) >= 0.f &&
						(v.y * dirNeg.y) >= 0.f &&
						(v.z * dirNeg.z) >= 0.f;

					if (isDir)
						associatedEnts.push_back(ent);
					break;
				}
				case StraddleCondition::AllOverlapping:
				{
					AABBBV& objBV = EntityManager::GetComponent<AABBBV>(ent);
					bool isCollide = Intersection::AABBAABBTest(temp.GetMinPoint(), temp.GetMaxPoint(), objBV.GetMinPoint(), objBV.GetMaxPoint());
					if (isCollide)
						associatedEnts.push_back(ent);
					break;
				}
				case StraddleCondition::CurrentLevel:
				{
					break;
				}
				case StraddleCondition::SplitObject:
				{
					break;
				}
				default: break;
			}
		}

		// Recurse into this new octant node
		auto& pOctantNode = pNode->Children[i];
		BVHTree_OctTree(pOctantNode, associatedEnts, newNodePos, newHalfE, height);
	}

	// updating height
	if (height > g.m_BVHHeight)
		g.m_BVHHeight = height;
}

void BVHSystem::BVHTree_KDTree(std::unique_ptr<BVHNode>& pNode, std::vector<std::pair<Entity, glm::vec3>>& points, int const axis, glm::vec3 const& nodePos, glm::vec3 const& nodeHalfE, int height)
{
	pNode = std::make_unique<BVHNode>();
	if (points.empty())
		return;

	std::sort(points.begin(), points.end(),
		[&axis](std::pair<Entity, glm::vec3> const& lhs, std::pair<Entity, glm::vec3> const& rhs)
		{
			return lhs.second[axis] < rhs.second[axis];
		});

	{ // Node data
		EntityList entities{}; entities.resize(points.size());
		for (size_t i = 0; i < points.size(); ++i)
			entities[i] = points[i].first;

		pNode->Handle = EntityManager::CreateEntity<Xform>();
		pNode->Objects = entities;
		pNode->Left.reset();
		pNode->Right.reset();
		pNode->SetIsNode();
	}
	{ // Node's BV data
		BoundingVolume& boundingVolume = EntityManager::AddComponent<BoundingVolume>(pNode->Handle, GetGlobalBVType());
		switch (boundingVolume.GetBVType())
		{
		case BV::AABB:
		{
			AABBBV& bv = EntityManager::GetComponent<AABBBV>(pNode->Handle);
			bv.SetPosition(nodePos);
			bv.GetHalfExtents() = nodeHalfE;
			break;
		}
		case BV::Sphere:
		{
			SphereBV& bv = EntityManager::GetComponent<SphereBV>(pNode->Handle);
			bv.SetPosition(nodePos);
			bv.GetRadius() = glm::compMax(nodeHalfE);
			break;
		}
		default: break;
		}
	}

	// Termination condition. Set this to a leaf.
	if (points.size() <= GetObjsPerNode() )//|| height >= GetMaxHeight())
	{
		pNode->SetIsLeaf();
		return;
	}

	static auto FindObjsOnPlane =
		[](std::vector<std::pair<Entity, glm::vec3>> const& points, int const axis, int const medianIndex, SplitPointStrat const strat)
		{
			EntityList ls{};
			float const& medianVal = points[medianIndex].second[axis];

			switch (strat)
			{
			case SplitPointStrat::MedianExtents:
			{
				// Stop before size-1 because we query i+1 without error handling
				for (size_t i = 0; i < (points.size() - 1); i += 2)
				{
					// min[axis] is points[i][axis], max[axis] is points[i+1][axis]
					float const& min = points[i].second[axis];
					float const& max = points[i+1].second[axis];

					if (min <= medianVal && medianVal <= max)
						ls.push_back(points[i].first);
				}
				break;
			}
			case SplitPointStrat::MedianCenters:
			default:
			{
				for (size_t i = 0; i < points.size(); ++i)
				{
					auto const& handle = points[i].first;
					AABBBV& bv = EntityManager::GetComponent<AABBBV>(handle);
					float const& min = bv.GetMinPoint()[axis];
					float const& max = bv.GetMaxPoint()[axis];

					// Get min and max [axis]
					if (min <= medianVal && medianVal <= max)
						ls.push_back(handle);
				}
				break;
			}
			}
			return ls;
		};

	int medianIndex = points.size() / 2;
	EntityList straddlers = FindObjsOnPlane(points, axis, medianIndex, GetCurrentSplitPointStrat());
	std::vector<std::pair<Entity, glm::vec3>> leftPoints{ points.begin(), points.begin() + medianIndex };
	std::vector<std::pair<Entity, glm::vec3>> rightPoints{ points.begin() + medianIndex, points.end() };
	std::remove_if(leftPoints.begin(), leftPoints.end(),
		[&straddlers](std::pair<Entity, glm::vec3>& point)
		{
			auto it = std::find(straddlers.begin(), straddlers.end(), point.first);
			return it != straddlers.end();
		});
	std::remove_if(rightPoints.begin(), rightPoints.end(),
		[&straddlers](std::pair<Entity, glm::vec3>& point)
		{
			auto it = std::find(straddlers.begin(), straddlers.end(), point.first);
			return it != straddlers.end();
		});

	int newAxis = (axis + 1) % 3;
	height += 1;
	glm::vec3 leftNodePos{}, leftHalfE{}, rightNodePos{}, rightHalfE{};
	{
		glm::vec3 nodeMin = nodePos - nodeHalfE;
		leftHalfE = nodeHalfE;
		leftHalfE[axis] = (points[medianIndex].second[axis] - nodeMin[axis]) * 0.5f;
		leftNodePos = nodeMin + leftHalfE;

		rightHalfE = nodeHalfE;
		rightHalfE[axis] = nodeHalfE[axis] - leftHalfE[axis];
		rightNodePos = leftNodePos;
		rightNodePos[axis] += leftHalfE[axis] + rightHalfE[axis];
	}

	BVHTree_KDTree(pNode->Left, leftPoints, newAxis, leftNodePos, leftHalfE, height);
	BVHTree_KDTree(pNode->Right, rightPoints, newAxis, rightNodePos, rightHalfE, height);

	// updating height
	if (height > g.m_BVHHeight)
		g.m_BVHHeight = height;
}
