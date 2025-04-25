#version 330 core
out vec4 oFragColor;
in vec2 oTexCoords;

uniform sampler2D uBaseTex;
uniform sampler2D uWireframeTex;

void main()
{ 
    //oFragColor = texture(screenTexture, oTexCoords);

    vec4 baseColor = texture(uBaseTex, oTexCoords);
    vec4 wireColor = texture(uWireframeTex, oTexCoords);
    // You can do additive, overlay, or conditional merge here
    oFragColor = max(baseColor, wireColor); // simple blend
}