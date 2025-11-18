#ifndef CRAPPYRENDERER_H
#define CRAPPYRENDERER_H
#include "BaseService.h"

namespace rdx
{
	enum class VertexAttributeType
	{
		Vec2, Vec3, Vec4,
		UInt8, UInt32, Float,
	};

	struct VertexAttribute
	{
		VertexAttributeType AttributeType{}; // eg vec4
		uint32_t AttributeCount{}; // Most would be 1, but say for Mat4, it has to be made of 4 vec4
		
		VertexAttributeType FundamentalType{}; // eg float
		uint32_t FundamentalCount{}; // Would be 4 floats for vec4

		uint32_t Location{}; // Shader location
		//uint32_t Offset{};

		bool IsInstanced{};
		bool IsNormalized{};
	};

	struct VertexLayout
	{
		std::vector<VertexAttribute> Attributes{};
	};

	class CrappyRenderer : public BaseService
	{
	public:
		bool InitImpl() override;
		bool TerminateImpl() override;

		void Draw();

	private:
		void SetupGlewDefaults();
		void SetupDefaultAssets();
	};
}

#endif