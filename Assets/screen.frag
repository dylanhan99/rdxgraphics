#version 330 core
out vec4 oFragColor;
in vec2 oTexCoords;

uniform sampler2D uBaseTex;
uniform bool uHasBaseTex;
uniform sampler2D uWireframeTex;
uniform bool uHasWireframeTex;

void main()
{ 
    //oFragColor = texture(screenTexture, oTexCoords);

    vec4 baseColor = texture(uBaseTex, oTexCoords);
    vec4 wireColor = texture(uWireframeTex, oTexCoords);
    // You can do additive, overlay, or conditional merge here

    if (uHasBaseTex && uHasWireframeTex)
        oFragColor = max(baseColor, wireColor); // simple blend
    else if (uHasBaseTex && !uHasWireframeTex)
        oFragColor = baseColor;
    else if (!uHasBaseTex && uHasWireframeTex)
        oFragColor = wireColor;
    else
        oFragColor = vec4(0.0, 1.0, 0.0, 1.0);
}