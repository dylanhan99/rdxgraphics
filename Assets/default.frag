#version 450 core

flat in uint oIsCollide;
out vec4 oFragColor;

uniform int uIsWireframe;
uniform vec3 uWireframeColor;

void main()
{
	if (uIsWireframe == 0)
		oFragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	else
	{
		oFragColor = vec4(1.0, 0.0,0.0,1.0);
		//oFragColor = vec4(float(oIsCollide), float(oIsCollide), float(oIsCollide), 1.0f);
		//if (oIsCollide == 0)
		//	oFragColor = vec4(uWireframeColor, 1.0f);
		//else
		//	oFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
}
