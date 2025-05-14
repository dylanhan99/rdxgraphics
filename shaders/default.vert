#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aXform;
layout (location = 7) in float aIsCollide;
layout (location = 8) in float aMatID;
layout (location = 9) in mat4 aMaterial;

layout (std140, binding=2) uniform MainCamera 
{
	mat4 ViewMatrix;
	mat4 ProjMatrix;
	vec4 Position;
	vec4 Direction; // Normalized
	vec2 Clip;
	vec2 ClipPadding;
};

struct Material
{
	vec3 AmbientColor;
	float AmbientIntensity;

	vec3 DiffuseColor;
	float DiffuseIntensity;

	vec3 SpecularColor;
	float SpecularIntensity;

	float Shininess;
	// vec3 padding; on shader end (std140)
};

out VS_OUT
{
	vec3 Position;
	vec2 TexCoords;
	vec3 Normal;
	flat float IsCollide;
	flat float MatID;
	flat Material Mat;
} vs_out;

Material MakeMat(mat4 m)
{
	Material mat;
	mat.AmbientColor		= m[0].xyz;
	mat.AmbientIntensity	= m[0].w;
	mat.DiffuseColor		= m[1].xyz;
	mat.DiffuseIntensity	= m[1].w;
	mat.SpecularColor		= m[2].xyz;
	mat.SpecularIntensity	= m[2].w;
	mat.Shininess			= m[3].x;
	return mat;
}

Material DefaultMaterial()
{
	return MakeMat(mat4(0.0));
}

void main()
{
	if (aMatID >= 1.0) // Has material
		vs_out.Mat = MakeMat(aMaterial);
	else
		vs_out.Mat = DefaultMaterial();
	vs_out.MatID = aMatID;

	vec4 model = aXform * vec4(aPos, 1.0);

	vs_out.Position	 = model.xyz;
	vs_out.TexCoords = aTexCoords;
	vs_out.Normal	 = mat3(transpose(inverse(aXform))) * aNormal;
	vs_out.IsCollide = aIsCollide;

	gl_Position = ProjMatrix * ViewMatrix * model;
}
