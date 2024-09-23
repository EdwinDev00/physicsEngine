#shader vertex
#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 tangent;



out vec2 v_TexCoord;

void main()
{
	v_TexCoord = texCoord;

	gl_Position = vec4(position,1);
}

#shader fragment
#version 430 core

struct DirLight {
    vec3 direction;
	vec3 color;
	float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
	float intensity;

    float constant;
    float linear;
    float quadratic;

	float radius;
};


layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedoSpec;
layout(binding = 3) uniform sampler2D pointlightResult;


uniform vec3 camPos; //Get the camera position from main

//Light Struct uniform
#define MAX_NR_POINT_LIGHTS 64
uniform int nrPointLight;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir,vec4 albedoSpecular);
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 crntPos, vec3 viewDir,vec4 albedoSpecular);
vec3 CalcPointLightSphere(PointLight light);

void main()
{
	vec3 crntPos = texture(gPosition, v_TexCoord).xyz;
	vec3 norm = normalize(texture(gNormal,v_TexCoord).xyz);
	vec4 albedoSpec = texture(gAlbedoSpec,v_TexCoord);

    vec3 viewDirection = normalize(camPos - crntPos);

	vec3 lightRes = CalcDirLight(dirLight,norm,viewDirection,albedoSpec);


	color = vec4(lightRes + texture(pointlightResult,v_TexCoord).xyz , 1);
}


vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec4 albedoSpecular)
{
	float ambientStrength = 0.1f;
	float shininess = 16; 

	vec3 lightDir = normalize(-light.direction);
	vec3 reflectDir = reflect(-lightDir,norm);

	//Calculate the light Strength
	float diffuseCoefficient = max(dot(lightDir,norm), 0.0f);
	float specularAmount = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

	//Combine the light Result
	vec3 ambient =  ambientStrength * light.color;
	vec3 diffuse =  (diffuseCoefficient) * light.color;
	vec3 specular = (specularAmount * albedoSpecular.a) * light.color;

	return (ambient + diffuse + specular) * albedoSpecular.rgb * light.intensity;
}