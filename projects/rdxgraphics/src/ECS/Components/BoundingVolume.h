#pragma once
#include "BaseComponent.h"
#include "Collider.h"

enum class BVState
{
	In,
	Out,
	On
};

class BoundingVolume : public BaseComponent
{
	RX_COMPONENT_HAS_HANDLE(BoundingVolume);
public:
	class DirtyXform : public BaseComponent { char _{}; };
	class DirtyBV : public BaseComponent { char _{}; };

public:
	inline BoundingVolume(BV bvType = BV::NIL) : m_BVType(bvType) {}
	inline void OnConstructImpl() override { SetupBV(); }
	inline void OnDestroyImpl() override { SetBVType(BV::NIL); }

	void RecalculateBV() const;

	inline BV GetBVType() const { return m_BVType; }
	void SetBVType(BV bvType);

	void SetDirty() const;
	void SetDirtyXform() const;

private:
	void SetupBV() const;
	void RemoveBV();

private:
	BV m_BVType{ BV::NIL };
};

class BaseBV : public virtual BasePrimitive
{
public:
	~BaseBV() = default;

	void OnConstructImpl() override { SetDirty(); }
	void SetDirtyXform() const override;
	void SetDirtyBV() const;
	virtual void RecalculateBV() = 0;
	inline void SetDirty() const { SetDirtyXform(); SetDirtyBV(); }

	inline BVState GetBVState() const { return m_BVState; }
	inline void SetBVState(BVState state) { m_BVState = state; }

private:
	BVState m_BVState{BVState::Out};
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

	inline std::array<glm::vec4, 8> const& GetPoints() const { return m_Points; }
	inline std::array<glm::mat4, 12> const& GetEdgeXforms() const { return m_Xforms; }
	inline std::array<glm::vec4, 6> const& GetPlaneEquations() const { return m_PlaneEquations; }
	inline std::array<glm::vec4, 6>& GetPlaneEquations() { return m_PlaneEquations; }

private:
	std::array<glm::vec4, 8> m_Points{};
	std::array<glm::mat4, 12> m_Xforms{};

	// 6 planes, generated via 8 points > frustum.
	std::array<glm::vec4, 6> m_PlaneEquations{};

};

class AABBBV : public BaseBV, public AABBPrimitive
{
	RX_COMPONENT_DEF_HANDLE(AABBBV);
public:
	AABBBV() = default;
	void RecalculateBV() override;
	void RecalculateBV(AABBBV const&); // This is more of a copy ctor
	void RecalculateBV(AABBBV const&, AABBBV const&);

private:

};

class OBBBV : public BaseBV, public AABBPrimitive
{
	RX_COMPONENT_DEF_HANDLE(OBBBV);
public:
	OBBBV() = default;
	void UpdateXform() override;
	void RecalculateBV() override;
	glm::mat3 const& GetOrthonormalBasis() const { return m_EigenVectors; }
	glm::mat3& GetOrthonormalBasis() { return m_EigenVectors; }

private:
	glm::mat3 m_EigenVectors{}; // ie Rotation
};

class SphereBV : public BaseBV, public SpherePrimitive
{
	RX_COMPONENT_DEF_HANDLE(SphereBV);
public:
	enum class Algo : int
	{
		Ritter,
		Larsson,
		PCA
	};

	inline static Algo Algorithm{ Algo::Ritter };

public:
	SphereBV() = default;
	void RecalculateBV() override;
	void RecalculateBV(SphereBV const&); // This is more of a copy ctor
	void RecalculateBV(SphereBV const&, SphereBV const&);

private:
};
