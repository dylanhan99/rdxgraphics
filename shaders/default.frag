#version 450 core

struct Material
{
	vec3 AmbientColor;
	float AmbientIntensity;

	vec3 DiffuseColor;
	float DiffuseIntensity;

	vec3 SpecularColor;
	float SpecularIntensity;

	float Shininess;
	// vec3 padding; on shader end (std140)
};

in vec3 oVtxPos;
in vec2 oTexCoords;
in vec3 oNormal;
flat in float oIsCollide;
flat in float oMatID;
flat in Material oMat;
out vec4 oFragColor;

uniform int uIsWireframe;
uniform vec3 uWireframeColor;
uniform vec3 uDirectionalLight;
uniform vec3 uViewPos;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    oFragColor = vec4(vec3(depth * 10.0), 1.0);
}

//void main()
//{
//	if (uIsWireframe == 0)
//	{
//		if (oMatID >= 1.0)
//		{
//			vec3 fragPos = oVtxPos;
//			vec3 norm = normalize(oNormal);
//			vec3 lightColor = vec3(1.0);
//			vec3 lightDir = normalize(-uDirectionalLight);
//
//			// Ambient
//			vec3 ambient = oMat.AmbientIntensity * lightColor;
//			//result = result *  oMat.AmbientColor;
//
//			// Diffuse
//			// vec3 lightDir = normalize(lightPos - FragPos);  
//			float diff = max(dot(norm, lightDir), 0.0);
//			vec3 diffuse = diff * vec3(1.0); // * lightColor
//
//			// Specular (Needs camera pos)
//			vec3 viewDir = normalize(uViewPos - fragPos);
//			vec3 reflectDir = reflect(-lightDir, norm);
//			float spec = pow(max(dot(viewDir, reflectDir), 0.0), oMat.Shininess);
//			vec3 specular = oMat.SpecularIntensity * spec * lightColor;  
//
//			// Out
//			vec3 result = (ambient + diffuse + specular) * oMat.AmbientColor;
//			oFragColor = vec4(result, 1.0);
//		}
//		else // No material
//			oFragColor = vec4(1.0, 0.075, 0.941, 1.0);
//	}
//	else
//	{
//		//oFragColor = vec4(1.0, 0.0,0.0,1.0);
//		//oFragColor = vec4(float(oIsCollide), float(oIsCollide), float(oIsCollide), 1.0f);
//		if (oIsCollide < 0.5)
//			oFragColor = vec4(uWireframeColor, 1.0f);
//		else
//			oFragColor = vec4(1.0, 0.0, 0.0, 1.0);
//	}
//}
