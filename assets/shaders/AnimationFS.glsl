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
			
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
uniform sampler2D u_NormalTexture;

uniform vec3 u_CameraPosition;
uniform int u_Shininess;


in vec3 v_FragPosition;
in vec2 v_TextureCoords;
in vec3 v_Normal;
in mat3 v_TBN;

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
	// PointLights
//	for (int i = 0; i < u_TotalPointLights; i++)
//	{
//		result += CalculatePointLight(u_PointLights[i], normal);
//	}

	// DirectionalLights
	for (int i = 0; i < u_TotalDirectionalLights; i++)
	{
		result += CalculateDirectionalLight(u_DirectionalLights[i], normal);
	}

	color = vec4(result * diffuseTexture.rgb, 1.0);
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