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

struct ForwardPlus	// think this throught ~ W-_-W ~
{
	int LightIndexList[500];
	vec2 CellLightInfo[256];
};

// MAX 1024 uniforms / shader
layout(location = 4) uniform uint nrOfPointLights;
layout(location = 8) uniform uint nrOfDirLights;
layout(location = 300) uniform PointLight pointLights[10];
layout(location = 500) uniform DirLight dirLights[10];

layout(location=800) uniform ForwardPlus forwardPlus;

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

//	// Calc Point Lights
//	for(int i = 0; i < nrOfPointLights; i++)
//	{
//		result += CalcPointLight(pointLights[i], normal, worldPos);
//	}

	int cellLocation = (16 * int(floor(gl_FragCoord.y / 45))) + int(floor(gl_FragCoord.x / 80));	// (gridSize * minY / cellHeight) + minX / cellWidth
	if (cellLocation >= 0 && cellLocation <= 255)
	{
		for(int i = int(forwardPlus.CellLightInfo[cellLocation].x) ; i < int(forwardPlus.CellLightInfo[cellLocation].x) + int(forwardPlus.CellLightInfo[cellLocation].y); i++)
		{

			result += CalcPointLight(pointLights[forwardPlus.LightIndexList[i]], normal, worldPos);
		}
	}
	// Calc Dir Lights
	for(int i = 0; i < nrOfDirLights; i++)
	{
		result += CalcDirLight(dirLights[i], normal);
	}

	// Add Ambient Light
	result + vec3(ambient);

	finalColor = vec4(result, 1.f);
	//finalColor = vec4(1.f, 1.f, 1.f, 1.f);


	// <<< HEAT MAP >>>

//	vec4 NDC = NDCPos;
//	NDC = NDC / NDC.z;
//	// Converting from NDC to pixel
//	int x = int(round((NDC.x + 1.0f) * Frustum.x / 2.0f));
//	int y = int(round((NDC.y + 1.0f) * Frustum.y / 2.0f));
//	int cellLocation = (16 * int(floor(y / 45))) + int(floor(x / 80));

//	int cellLocation = (16 * int(floor(gl_FragCoord.y / 45))) + int(floor(gl_FragCoord.x / 80));	// may cause the fps drop ~ W-_-W ~
//
//	if (cellLocation >= 0 && cellLocation <= 255)
//	{
//		if(forwardPlus.CellLightInfo[cellLocation].y == 1)
//		{
//			finalColor = vec4(0.f, 0.5f, 1.f, 1.f);
//		}
//		else if (forwardPlus.CellLightInfo[cellLocation].y == 2)
//		{
//			finalColor = vec4(0.f, 0.7f, 0.7f, 1.f);
//		}
//		else if (forwardPlus.CellLightInfo[cellLocation].y == 3)
//		{
//			finalColor = vec4(0.1f, 0.9f, 0.5f, 1.f);
//		}
//		else if (forwardPlus.CellLightInfo[cellLocation].y == 4)
//		{
//			finalColor = vec4(0.9f, 0.9f, 0.2f, 1.f);
//		}
//		else if (forwardPlus.CellLightInfo[cellLocation].y == 5)
//		{
//			finalColor = vec4(1.f, 0.5f, 0.f, 1.f);
//		}
//		else if (forwardPlus.CellLightInfo[cellLocation].y == 6)
//		{
//			finalColor = vec4(1.f, 0.f, 0.f, 1.f);
//		}
//		else
//		{
//			finalColor = vec4(0.f, 0.f, 0.8f, 1.f);
//		}
//	}
}