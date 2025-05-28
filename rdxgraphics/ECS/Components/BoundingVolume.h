#pragma once
#include "BaseComponent.h"
#include "Collider.h"

class BoundingVolume : public BaseComponent
{
	RX_COMPONENT_HAS_HANDLE(BoundingVolume);
public:
	class Dirty : public BaseComponent { char _{}; };

public:
	inline BoundingVolume(BV bvType = BV::NIL) : m_BVType(bvType) {}
	inline void OnConstructImpl() override { SetupBV(); }

	inline BV GetBVType() const { return m_BVType; }
	void SetBVType(BV bvType);

private:
	void SetupBV(glm::vec3 offset = glm::vec3{ 0.f });
	glm::vec3 RemoveBV(); // returns offset of previous BV

private:
	BV m_BVType{ BV::NIL };
};

class BaseBV : public virtual BasePrimitive
{
public:
	void OnConstructImpl() override { RX_WARN("BaseBV"); SetDirty(); }

private:
	void SetDirty() const;
	virtual void RecalculateBV() = 0;

private:

};

class AABBBV : public BaseBV, public AABBPrimitive
{
public:
	AABBBV() = default;
	void RecalculateBV() override;

private:

};