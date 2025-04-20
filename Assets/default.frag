#version 450 core

out vec4 oFragColor;

uniform int uIsWireframe;
uniform vec3 uWireframeColor;

void main()
{
	if (uIsWireframe == 0)
		oFragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	else
		oFragColor = vec4(uWireframeColor, 1.0f);
}
