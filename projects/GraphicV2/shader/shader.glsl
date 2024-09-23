#shader vertex
#version 330 core
			
layout(location = 0) in vec4 position;
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
	vec4 modelPos = vec4(u_Model[2] * position);
	crntPos = vec3(modelPos);
	gl_Position = u_MVP * position;

	//gl_Position = u_MVP * position;

	v_TexCoord = texCoord;
	Normal = aNormal;
};


#shader fragment
#version 330 core
	
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 Normal;
in vec3 crntPos;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform float u_lightIntensity;

//Get the camera position from main
uniform vec3 camPos;


void main()
{
	//Ambient lighting
	float ambient = 0.2f;

	//Diffuse
	vec3 normal = normalize(Normal);
	//Change the hardcodded vec3 to crntModelPosition
	vec3 lightDir = normalize(u_lightPos - crntPos); 
	float diffuse = max(dot(normal,lightDir), 0.0f);

	//Specular light
	float specularLight = 0.50f;
	vec3 viewDir = vec3(camPos - crntPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float specularAmount = pow(max(dot(viewDir,reflectionDir),0.0f),2);
	float specular = specularAmount * specularLight;

	vec4 texColor = texture(u_Texture,v_TexCoord);
	color = texColor * vec4(u_lightColor * u_lightIntensity, 1) * (diffuse + ambient + specular);
};
