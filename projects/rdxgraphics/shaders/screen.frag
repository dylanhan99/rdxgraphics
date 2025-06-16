#version 330 core
out vec4 oFragColor;
in vec2 oTexCoords;

/////
uniform sampler2D uModelsTex;
uniform sampler2D uCollidersTex;
uniform sampler2D uBVsTex;
uniform sampler2D uPiPModelsTex;
uniform sampler2D uPiPBVsTex;
/////
uniform bool uHasModels;
uniform bool uHasColliders;
uniform bool uHasBVs;
uniform bool uHasPiPModels;
uniform bool uHasPiPBVs;
/////

vec4 BlendColor(bool enabled, vec4 inColor, sampler2D tex, vec2 texCoords)
{
    if (!enabled) 
        return inColor;

    vec4 texColor = texture(tex, texCoords);
    return mix(inColor, texColor, texColor.a);
}

void main()
{ 
    vec4 finalColor;
    finalColor = BlendColor(uHasModels,     finalColor, uModelsTex,     oTexCoords);
    finalColor = BlendColor(uHasColliders,  finalColor, uCollidersTex,  oTexCoords);
    finalColor = BlendColor(uHasBVs,        finalColor, uBVsTex,        oTexCoords);
    finalColor = BlendColor(uHasPiPModels,  finalColor, uPiPModelsTex,  oTexCoords);
    finalColor = BlendColor(uHasPiPBVs,     finalColor, uPiPBVsTex,     oTexCoords);
    
    oFragColor = vec4(finalColor.rgb, 1.0);
}