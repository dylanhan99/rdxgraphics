#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec2 aNormal;
layout (location = 3) in mat4 aXform;
layout (location = 7) in float aIsCollide;
layout (location = 8) in float aMatID;
layout (location = 9) in mat4 aMaterial;

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

uniform mat4 uProjViewMatrix;
out vec2 oTexCoords;
flat out float oIsCollide;
flat out float oMatID;
flat out Material oMat;

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
	Material mat;
	mat.AmbientColor		= vec3(0.0);
	mat.AmbientIntensity	= 0.0;
	mat.DiffuseColor		= vec3(0.0);
	mat.DiffuseIntensity	= 0.0;
	mat.SpecularColor		= vec3(0.0);
	mat.SpecularIntensity	= 0.0;
	mat.Shininess			= 0.0;
	return mat;
}

void main()
{
	if (aMatID >= 1.0) // Has material
		oMat = MakeMat(aMaterial);
	else
		oMat = DefaultMaterial();
	oMatID = aMatID;

	oTexCoords = aTexCoords;
	oIsCollide = aIsCollide;
	gl_Position = uProjViewMatrix * aXform * vec4(aPos, 1.f);
}
