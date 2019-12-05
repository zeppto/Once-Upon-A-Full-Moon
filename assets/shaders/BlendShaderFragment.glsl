#version 440 core

layout(location = 0) out vec4 color;

struct PointLight
{
	vec3 Color;
	vec3 Position;
	float Radius;
	float Strength;
};
uniform int u_TotalPointLights;
uniform PointLight u_PointLights[10];

struct DirectionalLight
{
	vec3 Color;
	vec3 Direction;
	float Strength;
};
uniform int u_TotalDirectionalLights;
uniform DirectionalLight u_DirectionalLights[10];
	
	struct ForwardPlus
{
	int LightIndexList[3000];
	vec2 CellLightInfo[256];
};
uniform ForwardPlus forwardPlus;
uniform bool u_LightCulling;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_BlendMapTexture;
uniform sampler2D u_BlendTexture1;
uniform sampler2D u_BlendTexture2;

uniform vec3 u_CameraPosition;
uniform int u_Shininess;
//uniform float u_RepeatableTextureSize;	// Size of each "tile" on the terrain 


in vec3 v_FragPosition;
in vec2 v_TextureCoords;
in vec3 v_Normal;
in mat3 v_TBN;
in vec4 v_MVP_Position;

vec3 CalculatePointLight(PointLight light, vec3 normal);
vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal);

void main()
{
	// Textures
	vec4 normalTexture = texture(u_NormalTexture, 5 * v_TextureCoords);
	vec4 blendMap = texture(u_BlendMapTexture, v_TextureCoords);
	float diffuseTextureAmount = 1.f - (blendMap.r + blendMap.g + blendMap.b);						// How black is the fragment in blendMap
	vec4 diffuseTexture = texture(u_DiffuseTexture, 5 * v_TextureCoords) * diffuseTextureAmount;		// Extract diffuse color according to the amount of blackness
	vec4 blendTexture1 = texture(u_BlendTexture1, 5 * v_TextureCoords) * blendMap.r;					// Extract color depending on how much red there is in blendmap
	vec4 blendtexture2 = texture(u_BlendTexture2, 5 * v_TextureCoords) * blendMap.g;					// Extract color depending on how much green there is in blendmap

	vec4 finalTexture = diffuseTexture + blendTexture1 + blendtexture2;

	// Normal Map
	vec3 normal = normalize(normalTexture.rgb * 2.0 - 1.0);
	normal = normalize(v_TBN * normal);

	normal = max(normal, normalize(v_Normal));
	
	vec3 result = vec3(0.0, 0.0, 0.0);

	if(u_LightCulling)
	{
		vec3 NDC = v_MVP_Position.xyz / v_MVP_Position.w;					// Perspective divide/normalize
		vec2 viewportCoord = NDC.xy * 0.5 + 0.5;							// NDC is -1 to 1 in GL. scale for 0 to 1
		vec2 viewportPixelCoord;
	//	int cellLocation = (16 * int(floor(gl_FragCoord.y / 62.5f))) + int(floor(gl_FragCoord.x / 120.f));	// (gridSize * minY * cellHeight) + minX / cellWidth
		int cellLocation = int(16 * floor(16 * viewportCoord.y) + floor(16 * viewportCoord.x));
		
		if (cellLocation >= 0 && cellLocation <= 255)
		{
			// CellLocation x = offset		CellLocation y = size
			for(int i = int(forwardPlus.CellLightInfo[cellLocation].x); i < int(forwardPlus.CellLightInfo[cellLocation].x) + int(forwardPlus.CellLightInfo[cellLocation].y); i++)
			{
				result += CalculatePointLight(u_PointLights[forwardPlus.LightIndexList[i]], normal);
			}
		}
	}
	else
	{
		// PointLights
		for (int i = 0; i < u_TotalPointLights; i++)
		{
			result += CalculatePointLight(u_PointLights[i], normal);
		}
	}

	// DirectionalLights
	for (int i = 0; i < u_TotalDirectionalLights; i++)
	{
		result += CalculateDirectionalLight(u_DirectionalLights[i], normal);
	}

	color = vec4(result * finalTexture.rgb, 1.0);
}

vec3 CalculatePointLight(PointLight light, vec3 normal)
{
	float specularStrength = texture(u_SpecularTexture, v_TextureCoords).r;
	
	// Attenuation
	float dist = length(light.Position - v_FragPosition);
	float attenuation = smoothstep(light.Radius * 2, -1, dist);

	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * light.Color * attenuation;

	// Diffuse
	vec3 lightDir = normalize(light.Position - v_FragPosition);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.Color * attenuation * light.Strength;
	
	// Specular
	vec3 viewDir = normalize(u_CameraPosition - v_FragPosition);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
	vec3 specular = specularStrength * spec * light.Color * attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal)
{
	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * light.Color;

	// Diffuse
	vec3 lightDir = normalize(-light.Direction);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.Color * light.Strength;
	
	return (ambient + diffuse);
}