#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aXform;

uniform mat4 uProjViewMatrix;

void main()
{
	gl_Position = uProjViewMatrix * aXform * vec4(aPos, 1.f);
}
