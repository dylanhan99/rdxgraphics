#version 330 core
out vec4 oFragColor;
  
in vec2 oTexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    oFragColor = texture(screenTexture, oTexCoords);
}