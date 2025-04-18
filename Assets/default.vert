#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

void main()
{
	//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	gl_Position = uProjMatrix * uViewMatrix * vec4(aPos, 1.f);
}
