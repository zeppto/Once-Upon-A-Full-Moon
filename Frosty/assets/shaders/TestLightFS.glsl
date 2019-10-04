#version 440

out vec4 finalColor;

in vec3 worldPos;
in vec4 fragColor;

struct PointLight
{
	vec3 Position;
	vec4 Color;
	float Strength;
	float Radius;
	//vec2 linear_Quadratic;
};

struct DirLight
{
	vec4 Color;
	float Strength;
	vec3 Direction;
};

// MAX 1024 uniforms / shader
layout(location = 4) uniform uint nrOfPointLights;
layout(location = 100) uniform uint nrOfDirLights;
layout(location = 300) uniform PointLight pointLights[2];
layout(location = 500) uniform DirLight dirLights[2];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos)
{

    vec3 lightDir = normalize(light.Position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float distance = length(light.Position - fragPos);
	//float attenuation = 1.0 / (1.f + light.linear_Quadratic.x * distance + light.linear_Quadratic.y * (distance * distance));
	float attenuation = smoothstep(light.Radius * 2, -1, distance); // perform Hermite interpolation between two values		seems ok so far... ~ W-_-W ~
	vec3 diffuse = fragColor.rgb * light.Color.rgb * diff * light.Strength * attenuation /*(1.f/distance)*/;

	return (diffuse);
}

vec3 CalcDirLight(DirLight light, vec3 normal)
{
	vec3 lightDir = normalize(-light.Direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.Color.rgb * diff * light.Strength;

	return (diffuse);
}

void main()
{
	vec4 ambient = vec4(0.2f, 0.2f, 0.2f, 1.f);
	vec3 normal = vec3(0.f, 0.f, 1.f);
	vec3 result;

	// Calc Point Lights
	for(int i = 0; i < nrOfPointLights; i++)
	{
		result += CalcPointLight(pointLights[i], normal, worldPos);
	}
	// Calc Dir Lights
	for(int i = 0; i < nrOfDirLights; i++)
	{
		result += CalcDirLight(dirLights[i], normal);
	}

	// Add Ambient Light
	result + vec3(ambient);
	
	finalColor = vec4(result, 1.f);
}