#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aXform;
layout (location = 5) in uint aIsCollide;

uniform mat4 uProjViewMatrix;
flat out uint oIsCollide;

void main()
{
	oIsCollide = aIsCollide;
	gl_Position = uProjViewMatrix * aXform * vec4(aPos, 1.f);
}
