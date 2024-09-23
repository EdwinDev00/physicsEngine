#shader vertex
#version 430 core
			
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 aNormal;

out vec2 v_TexCoord;

out vec3 Normal;

//Will need to get the current model position for the light direction //should be vec3
out vec3 crntPos;
 
//Currently My camera MVP
uniform mat4 u_MVP; 
uniform mat4 u_Model;

void main()
{
	crntPos = vec3(u_Model * vec4(position,1.0f));
	gl_Position = u_MVP * vec4(position,1.0f);

	v_TexCoord = texCoord;
	Normal = mat3(u_Model) * aNormal;
};


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
};
	
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 Normal;
in vec3 crntPos;

uniform vec4 u_Color;


uniform vec3 camPos; //Get the camera position from main


//Light Struct uniform

#define NR_POINT_LIGHTS 64
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 crntPos, vec3 viewDir);

void main()
{
	vec3 norm = normalize(Normal);
    vec3 viewDirection = normalize(camPos - crntPos);

	vec3 lightRes = CalcDirLight(dirLight,norm,viewDirection);

	//Add multiple light
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		lightRes += CalcPointLight(pointLights[i],norm,crntPos,viewDirection);

	vec4 texColor = texture(u_Texture,v_TexCoord);
	color = texColor * vec4(lightRes, 0);
};

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir)
{
	float ambientStrength = 0.3f;
	float shininess = 32.0f; //Will be changed to use material shininess

	vec3 lightDir = normalize(-light.direction);
	vec3 reflectDir = reflect(-lightDir,norm);

	//Calculate the light Strength
	float diffuseCoefficient = max(dot(lightDir,norm), 0.0f) * light.intensity;
	float specularAmount = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

	//Combine the light Result
	vec3 ambient =  ambientStrength * light.color;
	vec3 diffuse =  diffuseCoefficient * light.color;
	vec3 specular = specularAmount * light.color;

	return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 norm, vec3 crntPos, vec3 viewDir)
{
	//Blin Phong
	float shininess = 32.0f;
	float ambientStrength = 0.1f;

	//Calculate the direction and the falloff of the light
	vec3 lightDir = normalize(light.position - crntPos); 
	float distance = length(light.position - crntPos);

	//Diffuse
	float diffuseCoefficient = max(dot(lightDir,norm), 0.0f);
	
	//Specular light
	vec3 halfwayDir = normalize(lightDir + viewDir); 
	float specularAmount = pow(max(dot(norm, halfwayDir), 0.0f), shininess);


	//Combine the result
	vec3 ambient = vec3(ambientStrength) * light.color;
	vec3 diffuse = (diffuseCoefficient * light.color) / (distance * distance);
	vec3 specular = specularAmount * light.color / (distance * distance);
	
	float attenuation = light.intensity / (light.constant + light.linear * distance + 
                light.quadratic * (distance * distance));
	
	return (ambient + diffuse + specular)  * attenuation;
}