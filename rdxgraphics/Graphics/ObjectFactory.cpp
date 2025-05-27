#include <pch.h>
#include "ObjectFactory.h"
#include "ECS/Components/Collider.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
	std::ios_base::openmode flags = std::ios::binary;
	std::ifstream ifs{ path, flags };
	if (!ifs)
	{
		RX_ERROR("Failed to open path - {}", path.string());
		return ObjectParams_VertexBasic{};
	}

	std::string buffer{ std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{} };
	return LoadModelBuffer(buffer);
}

ObjectParams_VertexBasic ObjectFactory::LoadModelBuffer(std::string const& buffer)
{
	Assimp::Importer importer{};
	uint32_t importFlags =
		aiProcess_CalcTangentSpace		|
		aiProcess_Triangulate			|
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals;

	const aiScene* pScene =
		importer.ReadFileFromMemory(buffer.data(), buffer.length(), importFlags);
	if (!pScene)
	{
		RX_ERROR("Failed to read buffer - {}", importer.GetErrorString());
		return ObjectParams_VertexBasic{};
	}

	std::vector<GLuint> indices{};
	VertexBasic::Position::container_type positions{};
	VertexBasic::Normal::container_type normals{};
	VertexBasic::TexCoord::container_type texCoords{};

	auto asd = std::function<void(aiNode*)>{};
	asd =
		[&](aiNode* pNode)
		{
			RX_ASSERT(pNode, "???");
			for (int i = 0; i < pNode->mNumMeshes; ++i)
			{
				uint32_t meshID = pNode->mMeshes[i];
				aiMesh* pMesh = pScene->mMeshes[meshID];
				std::cout << "asd";

				int numVertices = pMesh->mNumVertices;
				positions.resize(numVertices);
				normals.resize(numVertices);
				texCoords.resize(numVertices);
				std::memcpy(positions.data(), pMesh->mVertices, numVertices * sizeof(glm::vec3));
				std::memcpy(normals.data(), pMesh->mNormals, numVertices * sizeof(glm::vec3));

				//for (int j = 0; j < pMesh->mNumVertices; ++j)
				//{
				//	aiVector3D const& vert = pMesh->mVertices[j];
				//	aiVector3D const& norm = pMesh->mNormals[j];
				//	//aiVector3D const& cord = pMesh->mTextureCoords[j];
				//
				//	glm::vec3 pos{ vert.x, vert.y, vert.z };
				//	glm::vec3 nor{ norm.x, norm.y, norm.z };
				//	positions.emplace_back(pos);
				//	normals.emplace_back(nor);
				//	texCoords.emplace_back(glm::vec2{}); // later
				//}

				std::cout << "asd";

				// Assuming all are trianlges. Lazy and bad
				for (int k = 0; k < pMesh->mNumFaces; ++k)
				{
					indices.push_back(pMesh->mFaces[k].mIndices[0]);
					indices.push_back(pMesh->mFaces[k].mIndices[1]);
					indices.push_back(pMesh->mFaces[k].mIndices[2]);
				}
			}

			for (int i = 0; i < pNode->mNumChildren; ++i)
			{
				aiNode* pChild = pNode->mChildren[i];
				asd(pChild);
			}
		};

	//RX_ASSERT(pScene->HasMeshes(), "This object buffer has no meshes");
	// node -> multiple children -> multiple meshes 
	aiNode* node = pScene->mRootNode;
	asd(node);

	return ObjectParams_VertexBasic{
		GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

ObjectParams_VertexFBO ObjectFactory::CreateScreenQuad()
{
	std::vector<GLuint> indices{
		0, 1, 2,
		2, 3, 0
	};
	std::vector<glm::vec2> positions{
		{ -1.0f,  1.0f },
		{ -1.0f, -1.0f },
		{  1.0f, -1.0f },
		{  1.0f,  1.0f }
	};
	std::vector<glm::vec2> texCoords{
		{ 0.f, 1.f },
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 1.f, 1.f }
	};

	return ObjectParams_VertexFBO{
		GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		nullptr
	};
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

ObjectParams_VertexBasic ObjectFactory::SetupLine()
{
	// inward facing is the agreed upon standard for ray
	std::vector<GLuint> indices{
		0, 1,
	};
	VertexBasic::Position::container_type positions{
		{0.f,0.f,0.f}, {0.f,0.f,-1.f},
	};
	VertexBasic::TexCoord::container_type texCoords{};
	texCoords.resize(positions.size());
	VertexBasic::Normal::container_type normals{};
	normals.resize(positions.size(), RayPrimitive::DefaultDirection);

	return ObjectParams_VertexBasic{
		GL_LINES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

ObjectParams_VertexBasic ObjectFactory::SetupTriangle()
{
	// An equilateral triangle, with 1 unit from center to vertex
	VertexBasic::Position::container_type positions{
	TrianglePrimitive::DefaultP0,
	TrianglePrimitive::DefaultP1,
	TrianglePrimitive::DefaultP2
	};
	VertexBasic::TexCoord::container_type texCoords{
		glm::vec2{ (positions[0] + 1.f) * 0.5f },
		glm::vec2{ (positions[1] + 1.f) * 0.5f },
		glm::vec2{ (positions[2] + 1.f) * 0.5f },
	};
	VertexBasic::Normal::container_type normals{ 3, TrianglePrimitive::DefaultNormal };

	return ObjectParams_VertexBasic{
		GL_TRIANGLES,
		{},
		positions,
		texCoords,
		normals,
		nullptr
	};
}

ObjectParams_VertexBasic ObjectFactory::SetupQuad()
{
	std::vector<GLuint> indices{
		0, 1, 2,
		2, 3, 0
	};
	std::vector<glm::vec3> positions{
		{ -0.5f,  0.5f,  0.f },
		{ -0.5f, -0.5f,  0.f },
		{  0.5f, -0.5f,  0.f },
		{  0.5f,  0.5f,  0.f },
	};
	VertexBasic::TexCoord::container_type texCoords{
		{ 0.f, 1.f },
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 1.f, 1.f },
	};
	VertexBasic::Normal::container_type normals{};
	normals.resize(positions.size(), PlanePrimitive::DefaultNormal);

	return ObjectParams_VertexBasic{
		GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

ObjectParams_VertexBasic ObjectFactory::SetupPlane()
{ 
	// XY plane by default
	const uint32_t size = PlanePrimitive::DefaultSize;
	const float edgeLength = 1.f;
	std::vector<GLuint> indices{};
	std::vector<glm::vec3> positions{};

	float b = (float)size * edgeLength * 0.5f;
	glm::vec3 startPos{ -b, -b, 0.f };
	glm::vec3 step{ edgeLength, edgeLength, 0.f };

	for (uint32_t row = 0; row <= size; ++row)
	{
		float y = startPos.y + (float)row * step.y;
		for (uint32_t col = 0; col <= size; ++col)
		{
			float x = startPos.x + (float)col * step.x;
			positions.emplace_back(std::move(glm::vec3{ x, y, 0.f }));
		}
	}

	for (GLuint row = 0; row < size; ++row)
	{
		GLuint start = row * (size + 1);
		for (GLuint col = 0; col < size; ++col)
		{
			GLuint a = start + col + size + 1;
			GLuint b = start + col;
			GLuint c = start + col + 1;
			GLuint d = start + col + size + 1 + 1;

			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);

			indices.push_back(c);
			indices.push_back(d);
			indices.push_back(a);
		}
	}
	VertexBasic::TexCoord::container_type texCoords{};
	texCoords.resize(positions.size());
	VertexBasic::Normal::container_type normals{};
	normals.resize(positions.size(), PlanePrimitive::DefaultNormal);

	return ObjectParams_VertexBasic{
		GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

ObjectParams_VertexBasic ObjectFactory::SetupCube()
{
	std::vector<GLuint> indices{
		0, 1, 2, 2, 3, 0, // Front face
		4, 5, 6, 6, 7, 4, // Back face
		6, 5, 2, 2, 1, 6, // Bottom face
		0, 3, 4, 4, 7, 0, // Top face
		7, 6, 1, 1, 0, 7, // Left face
		3, 2, 5, 5, 4, 3  // Right face
	};
	VertexBasic::Position::container_type positions{
		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f }
	};
	VertexBasic::TexCoord::container_type texCoords{
		{ 0.f, 1.f },
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 1.f, 1.f },
		{ 0.f, 1.f },
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 1.f, 1.f }
	};

	VertexBasic::Normal::container_type normals{};
	// Generate normals.
	// Since we want normals, screw indexing and just duplicate all the vertices.
	// Kinda yucks but wtv man
	{
		VertexBasic::Position::container_type tempPositions{};
		VertexBasic::TexCoord::container_type tempTexCoords{};
		VertexBasic::Normal::container_type   tempNormals{};

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			GLuint i0 = indices[i];
			GLuint i1 = indices[i + 1];
			GLuint i2 = indices[i + 2];

			glm::vec3 const& p0 = positions[i0];
			glm::vec3 const& p1 = positions[i1];
			glm::vec3 const& p2 = positions[i2];
			glm::vec3 norm = glm::cross(p1 - p0, p2 - p1);

			tempPositions.push_back(p0);
			tempPositions.push_back(p1);
			tempPositions.push_back(p2);

			tempTexCoords.push_back(texCoords[i0]);
			tempTexCoords.push_back(texCoords[i1]);
			tempTexCoords.push_back(texCoords[i2]);

			tempNormals.push_back(norm);
			tempNormals.push_back(norm);
			tempNormals.push_back(norm);
		}

		std::swap(positions, tempPositions);
		std::swap(texCoords, tempTexCoords);
		std::swap(normals, tempNormals);
		indices.clear(); // 
	}

	return ObjectParams_VertexBasic{
		GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

ObjectParams_VertexBasic ObjectFactory::SetupSphere(int stacks, int sectors)
{
	if (stacks < 3) stacks = 3;
	if (sectors < 2) sectors = 2;

	const float radius = 1.f; const float radInv = 1.f / radius;
	const float stackStep = glm::pi<float>() / (float)stacks;
	const float sectorStep = glm::two_pi<float>() / (float)sectors;

	std::vector<GLuint> indices{};
	std::vector<glm::vec3> positions{};
	VertexBasic::TexCoord::container_type texCoords{};
	VertexBasic::Normal::container_type normals{};

	// x = (r * cos(phi)) * cos(theta)
	// y = (r * cos(phi)) * sin(theta)
	// z =  r * sin(phi)
	for (int i = 0; i <= stacks; ++i)
	{
		float stackAngle = glm::half_pi<float>() - (float)i * stackStep;
		float rcosP = radius * glm::cos(stackAngle);
		float z = radius * glm::sin(stackAngle);

		for (int j = 0; j <= sectors; ++j)
		{
			float sectorAngle = (float)j * sectorStep;
			float x = rcosP * glm::cos(sectorAngle);
			float y = rcosP * glm::sin(sectorAngle);

			glm::vec3 pos{ x, y, z };
			glm::vec2 tex{
				(float)j / sectors,
				(float)i / stacks
			};

			positions.push_back(pos);
			texCoords.push_back(tex);
			normals.push_back(pos * radInv);
		}
	}

	for (int i = 0; i < stacks; ++i)
	{
		GLuint k1 = i * (sectors + 1);
		GLuint k2 = k1 + sectors + 1;
		for (int j = 0; j < sectors; ++j)
		{
			if (i != 0)
			{
				indices.emplace_back(k1);
				indices.emplace_back(k2);
				indices.emplace_back(k1 + 1);
			}

			if (i != (stacks - 1))
			{
				indices.emplace_back(k1 + 1);
				indices.emplace_back(k2);
				indices.emplace_back(k2 + 1);
			}
			++k1;
			++k2;
		}
	}

	return ObjectParams_VertexBasic{
		GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}