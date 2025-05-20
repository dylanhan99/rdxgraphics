#version 330 core
out vec4 oFragColor;
in vec2 oTexCoords;

uniform sampler2D uBaseTex;
uniform bool uHasBaseTex;
uniform sampler2D uWireframeTex;
uniform bool uHasWireframeTex;
uniform sampler2D uMinimapTex;
uniform bool uHasMinimapTex;

void main()
{ 
    //oFragColor = texture(screenTexture, oTexCoords);

    vec4 baseColor = texture(uBaseTex, oTexCoords);
    vec4 wireColor = texture(uWireframeTex, oTexCoords);
    vec4 miniColor = texture(uMinimapTex, oTexCoords);

    vec4 finalColor;

    // I'd prefer the block below, but this just helps to force the back buffer color for now.
    // Lazy to change
    // You can do additive, overlay, or conditional merge here
    if (uHasBaseTex && uHasWireframeTex)
        finalColor = mix(baseColor, wireColor, wireColor.a); // simple blend
    else if (uHasBaseTex && !uHasWireframeTex)
        finalColor = baseColor;
    else if (!uHasBaseTex && uHasWireframeTex)
        finalColor = wireColor;
    else
        finalColor = vec4(1.0, 1.0, 0.0, 1.0);

    // Base
    //if (uHasBaseTex)
    //    finalColor = mix(finalColor, baseColor, baseColor.a);
    //// Wireframe
    //if (uHasWireframeTex)
    //    finalColor = mix(finalColor, wireColor, wireColor.a);
    // Minimap
    if (uHasMinimapTex)
        finalColor = mix(finalColor, miniColor, miniColor.a);

    // out
    oFragColor = vec4(finalColor.rgb, 1.0);
}