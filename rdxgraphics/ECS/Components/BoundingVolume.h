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

// Get inversed view matrix > apply to NDC points to get your 8 world points
// https://gamedev.stackexchange.com/questions/69749/get-the-8-corners-of-camera-frustrum
class FrustumBV : public BaseBV
{
	RX_COMPONENT_DEF_HANDLE(FrustumBV);
public:
	inline static const std::array<glm::vec4, 8> NDCPoints{
		// Near plane, TL > BL > BR > TR
		glm::vec4{-1.f, 1.f, 1.f, 1.f}, glm::vec4{-1.f, -1.f, 1.f, 1.f}, glm::vec4{1.f, -1.f, 1.f, 1.f}, glm::vec4{1.f, 1.f, 1.f, 1.f},
		// Far plane, TL > BL > BR > TR
		glm::vec4{-1.f, 1.f, -1.f, 1.f}, glm::vec4{-1.f, -1.f, -1.f, 1.f}, glm::vec4{1.f, -1.f, -1.f, 1.f}, glm::vec4{1.f, 1.f, -1.f, 1.f}
	};
public:
	FrustumBV() = default;
	void UpdateXform() override; // Must have it's own beacuse of BasePrimitive
	void RecalculateBV() override;

	inline std::array<glm::mat4, 12> const& GetEdgeXforms() const { return m_Xforms; }

private:
	std::array<glm::vec4, 8> m_Points{};
	std::array<glm::mat4, 12> m_Xforms{};
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
