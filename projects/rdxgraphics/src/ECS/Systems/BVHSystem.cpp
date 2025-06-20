#include "BVHSystem.h"

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

void BVHSystem::Init()
{
	EnforceUniformBVs();
}

void BVHSystem::EnforceUniformBVs()
{
	// Dirty BV handling needs to be updated to ensure proper 
	// calculation between NODES and LEAFS
	// Current idea: to calculate all leafs first, then work back up.
	// Can do this recursively via root node, then MakeBVH(lhs, rhs) which just combines the two.

	auto view = EntityManager::View<BoundingVolume>(entt::exclude<Camera>);
	for (auto [handle, bv] : view.each())
		bv.SetBVType(BVHSystem::GetGlobalBVType());
}

void BVHSystem::BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, std::vector<entt::entity> const& entities)
{ // page 240 (279 in pdf)
	// Re-set and recreate ALL BVs for leaf objects
	BVHNode* pp = new BVHNode{};
	pNode.reset(pp);

	if (entities.size() <= 1)
	{ // Leaf, just recalc, setup whatever vars in BVHNode
		entt::entity const handle = entities[0];
		EntityManager::GetComponent<BoundingVolume>(handle)
			.RecalculateBV();
		EntityManager::AddComponent<BVHNode::TypeLeaf>(handle);

		pp->Left.reset(nullptr);
		pp->Right.reset(nullptr);
	}
	else
	{ // Node, create entt::entity and add BV. Then calculate pos and bv data after recursing LHS and RHS
		entt::entity const handle = EntityManager::CreateEntity();
		EntityManager::AddComponent<Xform>(handle);
		EntityManager::AddComponent<BVHNode::TypeNode>(handle);
		EntityManager::AddComponent<BoundingVolume>(handle, GetGlobalBVType());

		// Split the entities
		// Based on some partition strategy,
		// entities[0, k-1] and entities[k, size-1]

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
