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
	switch (GetCurrentTreeType())
	{
	case BVHType::TopDown:
		BuildBVH(BVHTree_TopDown);
		break;
	default: break;
	}
}

void BVHSystem::BuildBVH(std::function<void(std::unique_ptr<BVHNode>&, Entity*, int)> fnBuildBVH)
{
	RX_ASSERT(fnBuildBVH);
	DestroyBVH(GetRootNode());

	std::vector<entt::entity> entities{};
	{
		auto view = EntityManager::View<Xform const, BoundingVolume const>(entt::exclude<FrustumBV>);
		for (auto [handle, _, __] : view.each())
			entities.emplace_back(handle);
	}

	EntityList sortedEnts = GetSortedEntities(entities);
	fnBuildBVH(GetRootNode(), sortedEnts.data(), (int)sortedEnts.size());
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

glm::vec3 BVHSystem::GetTreeAxis(std::vector<glm::vec3> const& positions)
{
	switch (GetCurrentTreeAxis())
	{
	case BVHAxis::X:
		return glm::vec3{ 1.f, 0.f, 0.f };
	case BVHAxis::Y:
		return glm::vec3{ 0.f, 1.f, 0.f };
	case BVHAxis::Z:
		return glm::vec3{ 0.f, 0.f, 1.f };
	case BVHAxis::PCA:
	{
		glm::mat3 eigenVecs{};
		Intersection::PCA(positions, nullptr, nullptr, &eigenVecs);
		return eigenVecs[0];
	}
	default: return {};
	}
}

BVHSystem::EntityList BVHSystem::GetSortedEntities(std::vector<entt::entity> const& entities)
{
	std::vector<glm::vec3> positions{};
	{
		for (auto const& handle : entities)
		{
			Xform& xform = EntityManager::GetComponent<Xform>(handle);
			positions.emplace_back(xform.GetTranslate());
		}
	}
	glm::vec3 axis = GetTreeAxis(positions);

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

	if (k == 0 || k >= numEnts)
		return (int)(numEnts / 2);

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

	if (numEnts <= GetMaxObjectsPerLeaf())
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
			_RX_X(AABB);
			_RX_X(OBB);
			_RX_X(Sphere);
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

		//std::vector<entt::entity> subset{};
		//std::ranges::transform(
		//	std::span(pEntities, pEntities + numEnts), 
		//	std::back_inserter(subset),
		//	[](Entity const& e)
		//	{
		//		return e.first;
		//	});
		//EntityList newSubset = GetSortedEntities(subset);
		//int k = Partition(newSubset.data(), numEnts);
		//
		//Entity* pEntitiesL = newSubset.data();
		//Entity* pEntitiesR = newSubset.data() + k;

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
	}

	// We still need the Xform to be updated, so leave it be. But BV is already 
	// calculated using RecalculateBV, so just force remove DirtyBV component
	{
		//EntityManager::RemoveComponent<BoundingVolume::DirtyXform>(handle);
		EntityManager::RemoveComponent<BoundingVolume::DirtyBV>(handle);
	}
}
