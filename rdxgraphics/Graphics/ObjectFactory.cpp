#include <pch.h>
#include "ObjectFactory.h"

template Object<VertexBasic> ObjectFactory::CreateObjekt(ObjectParams_VertexBasic const&);
template Object<VertexFBO> ObjectFactory::CreateObjekt(ObjectParams_VertexFBO const&);

template <typename T, typename U>
Object<T> ObjectFactory::CreateObjekt(U const& objParams)
{
	// Still gotta manually expand each .push if the vertex changes
	// No easy way to automatically inc the tuple's index through pre-proc. 
	// (not worth the time rn)
#define _RX_TUP(i) std::get<i>(objParams)

	Object<T> objekt{};
	objekt.BeginObject(_RX_TUP(0));

	auto& indices = _RX_TUP(1);
	if (indices.size())
		objekt.PushIndices(indices);

	// Push attribs, 
	// then validate all attribute containers should(?) be exactly the same size
	size_t sz = _RX_TUP(2).size();
	bool allSame = true;
#define _RX_X(Klass) .Push<Klass>(typename Klass::container_type{})
#define _RX_XX(Klass) allSame &= objekt.GetVBData<Klass>().size() == sz;
	if constexpr (std::is_same_v<T, VertexBasic>)
	{
		objekt
			.Push<VertexBasic::Position>(_RX_TUP(2))
			.Push<VertexBasic::TexCoord>(_RX_TUP(3))
			.Push<VertexBasic::Normal>  (_RX_TUP(4))
			RX_VERTEX_BASIC_ATTRIBS_M_INSTANCED(_RX_X)
			.EndObject();

		RX_VERTEX_BASIC_ATTRIBS_M_NOINSTANCED(_RX_XX);
	}
	else if constexpr (std::is_same_v<T, VertexFBO>)
	{
		objekt
			.Push<VertexFBO::Position>(_RX_TUP(2))
			.Push<VertexFBO::TexCoord>(_RX_TUP(3))
			RX_VERTEX_FBO_ATTRIBS_M_INSTANCED(_RX_X)
			.EndObject();

		RX_VERTEX_FBO_ATTRIBS_M_NOINSTANCED(_RX_XX);
	}
#undef _RX_XX
#undef _RX_X
#undef _RX_TUP
	RX_ASSERT(allSame, "Might have forgotten to setup an attribute's data for some object.");
	return std::move(objekt);
}

ObjectParams_VertexBasic ObjectFactory::LoadModelFile(std::filesystem::path const& path)
{
	return ObjectParams_VertexBasic();
}

ObjectParams_VertexBasic ObjectFactory::LoadModelBuffer(std::string const& buffer)
{
	return ObjectParams_VertexBasic();
}

ObjectParams_VertexBasic ObjectFactory::SetupPoint()
{
	std::vector<GLuint> indices{
	0, 1,
	2, 3,
	4, 5
	};
	VertexBasic::Position::container_type positions{
		{ -0.2f,  0.0f,  0.0f },  { 0.2f, 0.0f, 0.0f },
		{  0.0f, -0.2f,  0.0f },  { 0.0f, 0.2f, 0.0f },
		{  0.0f,  0.0f, -0.2f },  { 0.0f, 0.0f, 0.2f },
	};
	VertexBasic::TexCoord::container_type texCoords{};
	texCoords.resize(positions.size());
	VertexBasic::Normal::container_type normals{};
	normals.resize(positions.size());

	return ObjectParams_VertexBasic{
		GL_LINES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}
