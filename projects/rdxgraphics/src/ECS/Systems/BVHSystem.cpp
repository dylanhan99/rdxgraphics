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

int BVHSystem::Partition(Entity* pEntities, int numEnts)
{
	if (!pEntities || numEnts <= 0)
		return 0;

	if (numEnts == 2)
		return 1;

	// mean split
	float mean{};
	for (size_t i = 0; i < numEnts; ++i)
		mean += pEntities[i].second;
	mean /= (float)numEnts;

	// so now we partition based on lessthan or morethan centroid
	int k = 0;
	for (; k < (int)numEnts; ++k)
	{
		// we break as soon as we find that current >= mean
		if (pEntities[k].second >= mean)
			break;
	}

	return k;
}

void BVHSystem::BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* pEntities, int numEnts)
{
	if (numEnts <= 0 || !pEntities)
	{
		pNode.reset(nullptr);
		return;
	}

	pNode = std::make_unique<BVHNode>();

	if (numEnts == 1)
	{ // At an actual game object, we can simply use the entt handle and set LEAF
		entt::entity const& handle = pEntities[0].first;
		pNode->Handle = handle;
		pNode->SetIsLeaf();
	}
	else
	{ // Is a node,
		// Partition and recurse, then combine the children BVs to build your own
		// Set to NODE
		entt::entity const& handle = EntityManager::CreateEntity<Xform>();
		pNode->Handle = handle;
		pNode->SetIsNode();

		int k = Partition(pEntities, numEnts);
		Entity* pEntitiesL = pEntities;
		Entity* pEntitiesR = pEntities + k;

		BVHTree_TopDown(pNode->Left, pEntitiesL, k);
		BVHTree_TopDown(pNode->Right, pEntitiesR, numEnts - k);

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
			_RX_X(AABB);
			_RX_X(OBB);
			_RX_X(Sphere);
		default: break;
		}
#undef _RX_X
		// We still need the Xform to be updated, so leave it be. But BV is already 
		// calculated using RecalculateBV, so just force remove DirtyBV component
		{
			//EntityManager::RemoveComponent<BoundingVolume::DirtyXform>(handle);
			EntityManager::RemoveComponent<BoundingVolume::DirtyBV>(handle);
		}
	}
}
