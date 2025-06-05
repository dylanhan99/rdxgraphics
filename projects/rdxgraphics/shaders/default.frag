#version 450 core

struct CameraDS
{
	mat4 ViewMatrix;
	mat4 ProjMatrix;
	vec4 Position;
	vec4 Direction; // Normalized
	vec2 Clip;
	vec2 ClipPadding;
};

layout (std140, binding=2) uniform uCamera 
{
	CameraDS Camera;
};

in VS_OUT
{
	vec3 Position;
	vec2 TexCoords;
	vec3 Normal;
	flat float MatID;
	flat vec4 DiffuseColor;
} fs_in;

out vec4 oFragColor;

uniform vec4 uAmbientLight;
uniform int uIsWireframe;
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
	if (uIsWireframe == 0)
	{
		vec3 fragPos	= fs_in.Position;
		vec3 norm		= normalize(fs_in.Normal);
		vec3 lightColor = vec3(1.0);
		vec3 lightDir	= normalize(-uDirectionalLight);

		// Ambient
		vec3 ambient = uAmbientLight.xyz * uAmbientLight.w;
		//result = result *  oMat.AmbientColor;

		// Diffuse
		// vec3 lightDir = normalize(lightPos - FragPos);  
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;

		// Specular (Needs camera pos)
		//vec3 viewDir = normalize(vec3(cam.Position) - fragPos);
		//vec3 reflectDir = reflect(-lightDir, norm);
		//float spec = pow(max(dot(viewDir, reflectDir), 0.0), fs_in.Mat.Shininess);
		//vec3 specular = fs_in.Mat.SpecularIntensity * spec * lightColor;  
		vec3 specular = vec3(0.0);

		// Out
		vec3 result = (ambient + diffuse + specular) * (fs_in.DiffuseColor.xyz * fs_in.DiffuseColor.w);
		oFragColor = vec4(result, 1.0);
	}
	else
	{
		oFragColor = fs_in.DiffuseColor;
	}
}
