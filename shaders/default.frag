#version 450 core

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

in vec2 oTexCoords;
flat in float oIsCollide;
flat in float oMatID;
flat in Material oMat;
out vec4 oFragColor;

uniform int uIsWireframe;
uniform vec3 uWireframeColor;

void main()
{
	if (uIsWireframe == 0)
	{
		if (oMatID >= 1.0)
			oFragColor = vec4(oMat.AmbientColor, 1.0);
		else
			oFragColor = vec4(1.0, 0.075, 0.941, 1.0);
	}
	else
	{
		//oFragColor = vec4(1.0, 0.0,0.0,1.0);
		//oFragColor = vec4(float(oIsCollide), float(oIsCollide), float(oIsCollide), 1.0f);
		if (oIsCollide < 0.5)
			oFragColor = vec4(uWireframeColor, 1.0f);
		else
			oFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
}
