#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aXform;
layout (location = 7) in vec4 aDiffuseColor;

struct CameraDS
{
	mat4 ViewMatrix;
	mat4 ProjMatrix;
	vec4 Position;
	vec4 Direction; // Normalized
	vec2 Clip;
	vec2 ClipPadding;
};

layout (std140, binding=2) uniform uCamera 
{
	CameraDS Camera;
};

out VS_OUT
{
	vec3 Position;
	vec2 TexCoords;
	vec3 Normal;
	flat float MatID;
	flat vec4 DiffuseColor;
} vs_out;

void main()
{
	vec4 model = aXform * vec4(aPos, 1.0);

	vs_out.Position	 = model.xyz;
	vs_out.TexCoords = aTexCoords;
	vs_out.Normal	 = mat3(transpose(inverse(aXform))) * aNormal;
	vs_out.DiffuseColor = aDiffuseColor;

	gl_Position = Camera.ProjMatrix * Camera.ViewMatrix * model;
}
