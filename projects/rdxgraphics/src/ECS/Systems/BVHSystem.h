#pragma once
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

struct BVHNode
{
	struct TypeNode : public BaseComponent { char _{}; };
	struct TypeLeaf : public BaseComponent { char _{}; };

	entt::entity Handle{ entt::null };
	std::unique_ptr<BVHNode> Left{nullptr};
	std::unique_ptr<BVHNode> Right{nullptr};

	void SetIsNode() const;
	void SetIsLeaf() const;
	bool IsLeaf() const;
};

class BVHSystem : public BaseSingleton<BVHSystem>
{
	RX_SINGLETON_DECLARATION(BVHSystem);
private:
	using Entity = std::pair<entt::entity, float>; // handle, dotproduct on axis
	using EntityList = std::vector<Entity>;

public:
	static bool Init();
	static void EnforceUniformBVs();
	inline static std::unique_ptr<BVHNode>& GetRootNode() { return g.m_RootNode; }
	inline static BV& GetGlobalBVType() { return g.m_GlobalBVType; }

	// 
	static void BuildBVH(std::function<void(std::unique_ptr<BVHNode>&, Entity*, int)> fnBuildBVH);
	static void DestroyBVH(std::unique_ptr<BVHNode>& pNode);

	static EntityList GetSortedEntities(glm::vec3 const& axis);
	static int Partition(Entity* pEntities, int numEnts);
	//

	//
	static void BVHTree_TopDown(std::unique_ptr<BVHNode>& pNode, Entity* pEntities, int numEnts);
	//

private:

private:
	BV m_GlobalBVType{ BV::AABB };
	std::unique_ptr<BVHNode> m_RootNode{};
};