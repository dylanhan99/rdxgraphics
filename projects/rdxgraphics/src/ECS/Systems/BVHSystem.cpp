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

void BVHSystem::BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* entities, size_t numEnts)
{ // page 240 (279 in pdf)
	if (numEnts <= 0)
	{ // in case there arn't any entities for some reason
		pNode.reset(nullptr);
		return;
	}

	pNode = std::make_unique<BVHNode>();

	if (numEnts == 1)
	{ // Leaf, just recalc, setup whatever vars in BVHNode
		entt::entity const handle = (*entities).first;
		pNode->Handle = handle;

		EntityManager::GetComponent<BoundingVolume>(handle).RecalculateBV();
		EntityManager::AddComponent<BVHNode::TypeLeaf>(handle);

		pNode->Left.reset(nullptr);
		pNode->Right.reset(nullptr);
		// Ends the recursion for this leaf.
	}
	else
	{ // Node, create entt::entity and add BV. Then calculate pos and bv data after recursing LHS and RHS
		entt::entity const handle = EntityManager::CreateEntity();
		pNode->Handle = handle;

		EntityManager::AddComponent<Xform>(handle);
		EntityManager::AddComponent<BVHNode::TypeNode>(handle);
		BoundingVolume& boundingVolume = 
			EntityManager::AddComponent<BoundingVolume>(handle, GetGlobalBVType());
		// Can we remove the DirtyBV and DirtyBVXform here?

		// Split the entities
		// Based on some partition strategy,
		// entities[0, k-1] and entities[k, size-1]
		int k = PartitionEntities(entities, numEnts);
		Entity* entities_L = entities;		// [0, k-1]
		Entity* entities_R = entities + k;	// [k, numEnts-1]

		BVHTree_TopDown(pNode->Left,  entities_L, k);
		BVHTree_TopDown(pNode->Right, entities_R, numEnts - k);

		//if (pNode->Left && !pNode->Right)
		//{
		//
		//}
		//else if (!pNode->Left && pNode->Right)
		//{
		//
		//}
		//else
		{
			AABBBV bv = EntityManager::GetComponent<AABBBV>(handle);
			AABBBV bvL = EntityManager::GetComponent<AABBBV>(pNode->Left->Handle);
			AABBBV bvR = EntityManager::GetComponent<AABBBV>(pNode->Right->Handle);
			bv.RecalculateBV(bvL, bvR);
		}

		//switch (boundingVolume.GetBVType())
		//{
		//default: break;
		//}

		/*
		Klass##BV bv = GetComp<Klass##BV>(handle);
		BVTree_TopDown(pp->Left, leftEntities);
		BVTree_TopDown(pp->Right, rightEntities);

		Klass##BV bvL = GetComp<Klass##BV>(pp->Left->Handle);
		Klass##BV bvR = GetComp<Klass##BV>(pp->Right->Handle);

		bv.RecalculateByCombination(bvL, bvR);
		*/
	}
}

// Just destroy anything DO NOT recalculate anything here. 
// We'll take the inefficiency of not doing everything in place for the 
// sake of redability.
void BVHSystem::DestroyBVH(std::unique_ptr<BVHNode>& pNode)
{
	if (!pNode)
		return;

	entt::entity const& handle = pNode->Handle;
	if (EntityManager::HasComponent<BVHNode::TypeLeaf>(handle))
	{
		EntityManager::RemoveComponent<BVHNode::TypeLeaf>(handle);
	}
	if (EntityManager::HasComponent<BVHNode::TypeNode>(handle))
	{
		EntityManager::RemoveComponent<BVHNode::TypeNode>(handle);
		EntityManager::RemoveComponent<BoundingVolume>(handle);
	}

	DestroyBVH(pNode->Left);
	DestroyBVH(pNode->Right);
	pNode.release();
}

BVHSystem::EntityList BVHSystem::GetSortedEntities()
{
	std::vector<entt::entity> entities{};
	std::vector<glm::vec3> positions{};
	{
		auto view = EntityManager::View<Xform const, BoundingVolume const>(entt::exclude<FrustumBV>);
		for (auto [handle, xform, __] : view.each())
		{
			entities.emplace_back(handle);
			positions.emplace_back(xform.GetTranslate());
		}
	}

	glm::mat3 eigenVecs{};
	Intersection::PCA(positions, nullptr, nullptr, &eigenVecs);
	glm::vec3 const& principalAxis = eigenVecs[0];

	// Sorted along the principalAxis
	EntityList sortedEntities{};
	for (size_t i = 0; i < entities.size(); ++i)
	{
		float proj = glm::dot(positions[i], principalAxis);
		sortedEntities.emplace_back(std::make_pair(entities[i], proj));
	}

	// Sort in ascending order, the dot product
	std::sort(sortedEntities.begin(), sortedEntities.end(),
		[](Entity const& L, Entity const& R) -> bool
		{
			return L.second < R.second;
		});

	return std::move(sortedEntities);
}

int BVHSystem::PartitionEntities(Entity* entities, size_t numEnts)
{
	RX_ASSERT(entities);

	// mean split
	float mean{};
	for (size_t i = 0; i < numEnts; ++i)
		mean += entities[i].second;
	mean /= (float)numEnts;

	// so now we partition based on lessthan or morethan centroid
	int k = 0;
	for (; k < (int)numEnts; ++k)
	{
		// we break as soon as we find that current >= mean
		if (entities[k].second >= mean)
			break;
	}

	return k;
}
