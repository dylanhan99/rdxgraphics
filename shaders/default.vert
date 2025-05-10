#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in mat4 aXform;
layout (location = 6) in float aIsCollide;

uniform mat4 uProjViewMatrix;
out vec2 oTexCoords;
flat out float oIsCollide;

void main()
{
	oTexCoords = aTexCoords;
	oIsCollide = aIsCollide;
	gl_Position = uProjViewMatrix * aXform * vec4(aPos, 1.f);
}
