#pragma once
#include "BaseComponent.h"
#include "Collider.h"

class BoundingVolume : public BaseComponent
{
	RX_COMPONENT_HAS_HANDLE(BoundingVolume);
public:
	//class Dirty : public BaseComponent { char _{}; };

public:
	inline BoundingVolume(BV bvType = BV::NIL) : m_BVType(bvType) {}
	inline void OnConstructImpl() override { SetupBV(); }

	inline BV GetBVType() const { return m_BVType; }
	void SetBVType(BV bvType);

private:
	void SetupBV(glm::vec3 offset = glm::vec3{ 0.f }) const;
	glm::vec3 RemoveBV(); // returns offset of previous BV

private:
	BV m_BVType{ BV::NIL };
};

class BaseBV : public virtual BasePrimitive
{
public:
	void OnConstructImpl() override { SetDirty(); }

private:
	void SetDirty() const override;
	virtual void RecalculateBV() = 0;

private:

};

class FrustumBV : public BaseBV
{
	RX_COMPONENT_DEF_HANDLE(FrustumBV);
public:
	FrustumBV() = default;
	inline void UpdateXform() override {}; // Must have it's own beacuse of BasePrimitive
	inline void RecalculateBV() override {};

private:
};

class AABBBV : public BaseBV, public AABBPrimitive
{
	RX_COMPONENT_DEF_HANDLE(AABBBV);
public:
	AABBBV() = default;
	void RecalculateBV() override;

private:

};

class OBBBV : public BaseBV
{
	RX_COMPONENT_DEF_HANDLE(OBBBV);
public:
	OBBBV() = default;
	inline void UpdateXform() override {}; // Must have it's own beacuse of BasePrimitive
	inline void RecalculateBV() override {};

private:
};

class SphereBV : public BaseBV, public SpherePrimitive
{
	RX_COMPONENT_DEF_HANDLE(SphereBV);
public:
	SphereBV() = default;
	inline void RecalculateBV() override;

private:
};
