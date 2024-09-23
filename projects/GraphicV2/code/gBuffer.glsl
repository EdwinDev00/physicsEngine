#shader vertex
#version 430 core
			
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aTangent;

out vec2 v_TexCoord;

out vec3 Normal;
out mat3 TBN;

out vec3 crntPos;
 
//Currently My camera MVP
uniform mat4 u_MVP; 
uniform mat4 u_Model;

void main()
{
	mat3 model3 =  mat3(u_Model);
	vec3 T = normalize(model3 * aTangent.xyz);
	Normal = normalize(model3 * aNormal);
	vec3 B = normalize(cross(Normal,T)) * aTangent.w;

	TBN = mat3(T,B,Normal);

	crntPos = vec3(u_Model * vec4(position,1.0f));
	gl_Position = u_MVP * vec4(position,1.0f);

	v_TexCoord = texCoord;
};


#shader fragment
#version 430 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec2 v_TexCoord;
in vec3 Normal;
in mat3 TBN;
in vec3 crntPos;

uniform vec4 u_Color;
layout(binding = 0) uniform sampler2D u_Texture;
layout(binding = 1) uniform sampler2D u_NormalTex;
layout(binding = 2) uniform sampler2D u_MetalRoughnessTex;


void main()
{
	vec4 texColor = texture(u_Texture,v_TexCoord);
	if(texColor.a <  0.1)
	{
		discard;
	}

	vec3 norm;
	if (isnan(TBN[0][0])) // if tangents are 0 just use vertex normals
    {
        norm = normalize(TBN[2]);
    }
    else
    {
        norm = texture(u_NormalTex, v_TexCoord).rgb;
        norm = normalize(norm * 2 - 1);
        norm = normalize(TBN * norm);
    }

	gPosition = crntPos;
	gNormal = norm;
	gAlbedoSpec = texColor;
	gAlbedoSpec.a = texture(u_MetalRoughnessTex,v_TexCoord).g;
	
};

