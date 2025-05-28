#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aXform;
layout (location = 7) in float aIsCollide;
layout (location = 8) in vec4 aDiffuseColor;

struct Camera
{
	mat4 ViewMatrix;
	mat4 ProjMatrix;
	vec4 Position;
	vec4 Direction; // Normalized
	vec2 Clip;
	vec2 ClipPadding;
};

layout (std140, binding=2) uniform MainCamera 
{
	Camera Cameras[2];
};

uniform int uCam;

out VS_OUT
{
	vec3 Position;
	vec2 TexCoords;
	vec3 Normal;
	flat float IsCollide;
	flat float MatID;
	flat vec4 DiffuseColor;
} vs_out;

void main()
{
	Camera cam = Cameras[uCam];
	vec4 model = aXform * vec4(aPos, 1.0);

	vs_out.Position	 = model.xyz;
	vs_out.TexCoords = aTexCoords;
	vs_out.Normal	 = mat3(transpose(inverse(aXform))) * aNormal;
	vs_out.IsCollide = aIsCollide;
	vs_out.DiffuseColor = aDiffuseColor;

	gl_Position = cam.ProjMatrix * cam.ViewMatrix * model;
}
