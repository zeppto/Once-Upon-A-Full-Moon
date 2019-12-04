#version 440 core

//NOTE: Identical to texture2DFragment.
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

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
uniform sampler2D u_NormalTexture;

uniform vec3 u_CameraPosition;
uniform int u_Shininess;
uniform vec4 u_ObjectColor;
uniform float u_Flash;

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
	vec4 diffuseTexture = texture(u_DiffuseTexture, v_TextureCoords);
	vec4 normalTexture = texture(u_NormalTexture, v_TextureCoords);

	vec3 normal = normalize(normalTexture.rgb * 2.0 - 1.0);
	normal = normalize(v_TBN * normalTexture.rgb);

	normal = max(normal, normalize(v_Normal));
	
	vec3 result = vec3(0.0, 0.0, 0.0);

	vec3 NDC = v_MVP_Position.xyz / v_MVP_Position.w;					// Perspective divide/normalize
	vec2 viewportCoord = NDC.xy * 0.5 + 0.5;							// NDC is -1 to 1 in GL. scale for 0 to 1
	vec2 viewportPixelCoord;
	int cellLocation = int(16 * floor(16 * viewportCoord.y) + floor(16 * viewportCoord.x));
	if (cellLocation >= 0 && cellLocation <= 255)
	{
		// CellLocation x = offset		CellLocation y = size
		for(int i = int(forwardPlus.CellLightInfo[cellLocation].x); i < int(forwardPlus.CellLightInfo[cellLocation].x) + int(forwardPlus.CellLightInfo[cellLocation].y); i++)
		{
			result += CalculatePointLight(u_PointLights[forwardPlus.LightIndexList[i]], normal);
		}
	}

	// DirectionalLights
	for (int i = 0; i < u_TotalDirectionalLights; i++)
	{
		result += CalculateDirectionalLight(u_DirectionalLights[i], normal);
	}

	color = vec4(result * diffuseTexture.rgb, 1.0) + u_Flash;
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