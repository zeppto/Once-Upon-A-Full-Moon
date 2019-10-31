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

uniform vec3 u_CameraPosition;
uniform vec4 u_ObjectColor;
uniform float u_SpecularStrength;
uniform int u_Shininess;

in vec3 v_FragPosition;
in vec3 v_Normal;

vec3 CalculatePointLight(PointLight light);
vec3 CalculateDirectionalLight(DirectionalLight light);

void main()
{
	vec3 result = vec3(0.0, 0.0, 0.0);

	// PointLights
	for (int i = 0; i < u_TotalPointLights; i++)
	{
		result += CalculatePointLight(u_PointLights[i]);
	}

	// DirectionalLights
	for (int i = 0; i < u_TotalDirectionalLights; i++)
	{
		result += CalculateDirectionalLight(u_DirectionalLights[i]);
	}

	color = vec4(result * u_ObjectColor.rgb, 1.0f);
}


vec3 CalculatePointLight(PointLight light)
{
	// Attenuation
	float dist = length(light.Position - v_FragPosition);
	float attenuation = smoothstep(light.Radius * 2, -1, dist);

	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * light.Color * attenuation;

	// Diffuse
	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(light.Position - v_FragPosition);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.Color * attenuation * light.Strength;
	
	// Specular
	vec3 viewDir = normalize(u_CameraPosition - v_FragPosition);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
	vec3 specular = u_SpecularStrength * spec * light.Color * attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalculateDirectionalLight(DirectionalLight light)
{
	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * light.Color;

	// Diffuse
	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(-light.Direction);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.Color * light.Strength;

	return ambient + diffuse;
}