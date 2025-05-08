#pragma once

class BaseComponent
{
public:
	~BaseComponent() = default;
private:

};

class Xform : public BaseComponent
{
public:
	Xform() = default;
	inline Xform(glm::vec3 const& pos) : m_Translate(pos) {}

	void UpdateXform();

	inline glm::mat4 const& GetXform() const { return m_Xform; }
	inline glm::mat4& GetXform() { return m_Xform; }
	inline glm::vec3 const& GetTranslate() const { return m_Translate; }
	inline glm::vec3& GetTranslate() { return m_Translate; }
	inline glm::vec3 const& GetScale() const { return m_Scale; }
	inline glm::vec3& GetScale() { return m_Scale; }
	inline glm::vec3 const& GetEulerOrientation() const { return m_Rotate; }
	inline glm::vec3& GetEulerOrientation() { return m_Rotate; }

private:
	glm::vec3 m_Translate{ 0.f }, m_Scale{ 1.f }, m_Rotate{ 0.f };
	glm::mat4 m_Xform{};
};

class Model : public BaseComponent
{
public:
	Model() = default;
	inline Model(Rxuid uid) : m_MeshID(uid) {}
	inline Model(Shape s) : m_MeshID(Rxuid{ s }) {}

	inline Rxuid GetMesh() const { return m_MeshID; }
	inline void SetMesh(Rxuid uid) { m_MeshID = uid; }

private:
	Rxuid m_MeshID{ Rxuid{ Shape::NIL } };
	//Shape ShapeType{ Shape::Quad };
};

class Collider : public BaseComponent
{
public:
	Collider() = default;

	template <typename T, typename ...Args>
	//inline Collider(Args&& ...args) { SetBV<T>(args...); }
	//inline Collider(BV bvType) { SetBV(bvType); }

	inline BV GetBVType() const { return m_BVType; }

	//template <typename T, typename ...Args>
	//void SetBV(Args&& ...args)
	//{
	//	if (GetBVType() != BV::NIL)
	//	{
	//		// Remove the old one,
	//	}
	//
	//	m_BVType = GetBVType<T>();
	//}
	//inline void SetBV(BV bvType);

private:
	BV m_BVType{ BV::NIL };
};

//class BaseBoundingVolume
//{
//	
//};
//
//class RayBV : public BaseBoundingVolume
//{
//
//};