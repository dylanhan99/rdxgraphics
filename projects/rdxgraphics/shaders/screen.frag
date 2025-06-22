#version 330 core
out vec4 oFragColor;
in vec2 oTexCoords;

/////
uniform sampler2D uModelsTex;
uniform sampler2D uCollidersTex;
uniform sampler2D uBVLeafsTex;
uniform sampler2D uBVHTex;
uniform sampler2D uPiPModelsTex;
uniform sampler2D uPiPBVLeafsTex;
/////
uniform bool uHasModels;
uniform bool uHasColliders;
uniform bool uHasBVLeafs;
uniform bool uHasBVH;
uniform bool uHasPiPModels;
uniform bool uHasPiPBVLeafs;
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
    finalColor = BlendColor(uHasBVLeafs,    finalColor, uBVLeafsTex,    oTexCoords);
    finalColor = BlendColor(uHasBVH,        finalColor, uBVHTex,        oTexCoords);
    finalColor = BlendColor(uHasPiPModels,  finalColor, uPiPModelsTex,  oTexCoords);
    finalColor = BlendColor(uHasPiPBVLeafs, finalColor, uPiPBVLeafsTex, oTexCoords);
    
    oFragColor = vec4(finalColor.rgb, 1.0);
}