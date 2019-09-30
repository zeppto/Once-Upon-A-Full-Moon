#version 440

layout(binding = 0) uniform sampler2D tex;

out vec4 finalColor;

layout(location = 0) in vec3 vsOutPos;
layout(location = 1) in vec2 vsOutUV;
layout(location = 2) in vec3 vsOutNormal; // color/normal


struct PointLight
{
	vec3 position;
	vec4 color;
	float strength;
	vec2 linear_Quadratic;
};

struct DirLight
{
	vec4 color;
	float strength;
	vec3 direction;
};

// MAX 1024 uniforms / shader
layout(location = 3) uniform uint nrOfPointLights;
layout(location = 100) uniform uint nrOfDirLights;
layout(location = 300) uniform PointLight pointLights[2];
layout(location = 500) uniform DirLight dirLights[2];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (1.f + light.linear_Quadratic.x * distance + light.linear_Quadratic.y * (distance * distance));
	vec3 diffuse = vsOutNormal * light.color.rgb * diff * light.strength * attenuation /*(1.f/distance)*/;

	return (diffuse);
}

vec3 CalcDirLight(DirLight light, vec3 normal)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = vsOutNormal * light.color.rgb * diff * light.strength;

	return (diffuse);
}

void main()
{
	vec4 ambient = vec4(0.2f, 0.2f, 0.2f, 1.f);
	vec3 normal = vec3(0.f, 0.f, 1.f);
	vec3 result;
	

	vec4 diffTexture = vec4((texture(tex, vec2(vsOutUV.x, -vsOutUV.y)).xyz), 1.0);

	// Calc Point Lights
	for(int i = 0; i < nrOfPointLights; i++)
	{
		result += CalcPointLight(pointLights[i], normal, vsOutPos);
	}
	// Calc Dir Lights
	for(int i = 0; i < nrOfDirLights; i++)
	{
		result += CalcDirLight(dirLights[i], normal);
	}
	// Add Ambient Light
	result + vec3(ambient);

	//finalColor = diffTexture*vec4(result, 1.f);
	finalColor = diffTexture;
	//finalColor = diffTexture*vec4(result, 1.f);


}