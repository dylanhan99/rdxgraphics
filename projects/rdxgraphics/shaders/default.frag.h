R"(
#version 450 core
struct Camera
{
	mat4 ViewMatrix;
	mat4 ProjMatrix;
	vec4 Position;
	vec4 Direction; // Normalized
	vec2 Clip;
	vec2 ClipPadding;
};

layout (std140, binding=2) uniform MainCamera 
{
	Camera Cameras[2];
};

uniform int uCam;

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

in VS_OUT
{
	vec3 Position;
	vec2 TexCoords;
	vec3 Normal;
	flat float IsCollide;
	flat float MatID;
	flat Material Mat;
} fs_in;

out vec4 oFragColor;

uniform int uIsWireframe;
uniform vec3 uWireframeColor;
uniform vec3 uDirectionalLight;

float LinearizeDepth(float depth, float near, float far) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

//void main()
//{             
//    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
//    oFragColor = vec4(vec3(depth * 10.0), 1.0);
//}

void main()
{
	Camera cam = Cameras[uCam];

	if (uIsWireframe == 0)
	{
		if (fs_in.MatID >= 1.0)
		{
			vec3 fragPos	= fs_in.Position;
			vec3 norm		= normalize(fs_in.Normal);
			vec3 lightColor = vec3(1.0);
			vec3 lightDir	= normalize(-uDirectionalLight);

			// Ambient
			vec3 ambient = fs_in.Mat.AmbientIntensity * lightColor;
			//result = result *  oMat.AmbientColor;

			// Diffuse
			// vec3 lightDir = normalize(lightPos - FragPos);  
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = diff * vec3(1.0); // * lightColor

			// Specular (Needs camera pos)
			vec3 viewDir = normalize(vec3(cam.Position) - fragPos);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), fs_in.Mat.Shininess);
			vec3 specular = fs_in.Mat.SpecularIntensity * spec * lightColor;  

			// Out
			vec3 result = (ambient + diffuse + specular) * fs_in.Mat.AmbientColor;
			oFragColor = vec4(result, 1.0);
		}
		else // No material
			oFragColor = vec4(1.0, 0.075, 0.941, 1.0);
	}
	else
	{
		//oFragColor = vec4(1.0, 0.0,0.0,1.0);
		//oFragColor = vec4(float(oIsCollide), float(oIsCollide), float(oIsCollide), 1.0f);
		if (fs_in.IsCollide < 0.5)
			oFragColor = vec4(uWireframeColor, 1.0f);
		else
			oFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
}
)"