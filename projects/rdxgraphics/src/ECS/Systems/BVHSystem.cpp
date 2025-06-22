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

void BVHSystem::BuildBVH(std::function<void(std::unique_ptr<BVHNode>&, Entity*, int)> fnBuildBVH)
{
	RX_ASSERT(fnBuildBVH);

	DestroyBVH(GetRootNode());
	glm::vec3 axis{ 1.f, 0.f, 0.f }; // X-axis
	EntityList sortedEnts = GetSortedEntities(axis);
	fnBuildBVH(GetRootNode(), sortedEnts.data(), (int)sortedEnts.size());
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
		EntityManager::Destroy(handle);

		DestroyBVH(pNode->Left);
		DestroyBVH(pNode->Right);
	}
	pNode.release();
}

BVHSystem::EntityList BVHSystem::GetSortedEntities(glm::vec3 const& axis)
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

	EntityList sortedEnts{};
	for (size_t i = 0; i < entities.size(); ++i)
	{
		sortedEnts.emplace_back(Entity{
			entities[i],
			glm::dot(axis, positions[i])
			});
	}

	std::sort(sortedEnts.begin(), sortedEnts.end(),
		[](Entity const& lhs, Entity const& rhs)
		{
			return lhs.second < rhs.second;
		});

	return std::move(sortedEnts);
}

void BVHSystem::BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* pEntities, int numEnts)
{
}
