#version 450 core
layout(location = 0) out vec4 oFragColor;

uniform vec3 uWireframeColor;

void main()
{
	oFragColor = vec4(uWireframeColor, 1.0);
	//oFragColor = vec4(0.0,1.0,0.0, 1.0);
}
