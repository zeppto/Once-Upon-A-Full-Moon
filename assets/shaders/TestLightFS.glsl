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
uniform PointLight u_PointLights[20];

struct DirectionalLight
{
	vec3 Color;
	vec3 Direction;
	float Strength;
};
uniform int u_TotalDirectionalLights;
uniform DirectionalLight u_DirectionalLights[5];

struct ForwardPlus
{
	int LightIndexList[3000];
	vec2 CellLightInfo[256];
};
uniform ForwardPlus forwardPlus;
uniform int u_HeatMap;

uniform vec3 u_CameraPosition;
uniform vec4 u_ObjectColor;
uniform float u_SpecularStrength;
uniform int u_Shininess;

in vec3 v_FragPosition;
in vec3 v_Normal;
in vec4 v_MVP_Position;

vec3 CalculatePointLight(PointLight light);
vec3 CalculateDirectionalLight(DirectionalLight light);

void main()
{
	vec3 result = vec3(0.1f, 0.1f, 0.5f);

	vec3 NDC = v_MVP_Position.xyz / v_MVP_Position.w;					// Perspective divide/normalize
	vec2 viewportCoord = NDC.xy * 0.5 + 0.5;							// NDC is -1 to 1 in GL. scale for 0 to 1
	vec2 viewportPixelCoord;

//	int cellLocation = (16 * int(floor(gl_FragCoord.y / 62.5f))) + int(floor(gl_FragCoord.x / 120.f));	// (gridSize * minY * cellHeight) + minX / cellWidth
	int cellLocation = int(16 * floor(16 * viewportCoord.y) + floor(16 * viewportCoord.x));
//	
	if(u_HeatMap == 0)
	{
		if (cellLocation >= 0 && cellLocation <= 255)
		{
			// CellLocation x = offset		CellLocation y = size
			for(int i = int(forwardPlus.CellLightInfo[cellLocation].x); i < int(forwardPlus.CellLightInfo[cellLocation].x) + int(forwardPlus.CellLightInfo[cellLocation].y); i++)
			{
				result += CalculatePointLight(u_PointLights[forwardPlus.LightIndexList[i]]);
			}
		}
		color = vec4(result, 1.f);
	}


//				 <<< HEAT MAP >>>
	else if(u_HeatMap == 1)
	{
		if (cellLocation >= 0 && cellLocation <= 255)
		{
			if(forwardPlus.CellLightInfo[cellLocation].y == 1)
			{
				color = vec4(0.f, 0.f, 0.5f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 2)
			{
				color = vec4(0.1f, 0.f, 1.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 3)
			{
				color = vec4(0.f, 0.3f, 1.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 4)
			{
				color = vec4(0.f, 0.5f, 1.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 5)
			{
				color = vec4(0.f, 0.5f, 0.5f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 6)
			{
				color = vec4(0.f, 0.5f, 0.1f, 1.f);
			}
			else if(forwardPlus.CellLightInfo[cellLocation].y == 7)
			{
				color = vec4(0.f, 0.5f, 0.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 8)
			{
				color = vec4(0.1f, 0.7f, 0.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 9)
			{
				color = vec4(0.4f, 1.f, 0.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 10)
			{
				color = vec4(0.7f, 1.f, 0.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 11)
			{
				color = vec4(1.f, 1.f, 0.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 12)
			{
				color = vec4(1.f, 0.7f, 0.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 13)
			{
				color = vec4(1.f, 0.4f, 0.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 14)
			{
				color = vec4(1.f, 0.0f, 0.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 15)
			{
				color = vec4(1.f, 0.0f, 0.5f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 16)
			{
				color = vec4(1.f, 0.0f, 0.7f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 17)
			{
				color = vec4(1.f, 0.0f, 1.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 18)
			{
				color = vec4(0.7f, 0.0f, 1.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 19)
			{
				color = vec4(0.5f, 0.0f, 1.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 20)
			{
				color = vec4(0.1f, 0.0f, 1.f, 1.f);
			}
			else if (forwardPlus.CellLightInfo[cellLocation].y == 0)
			{
				color = vec4(0.f, 0.f, 0.f, 1.f);
			}

//			if(u_TotalPointLights == 1)
//			{
//				color = vec4(0.f, 0.f, 0.5f, 1.f);
//			}
//			else if (u_TotalPointLights == 2)
//			{
//				color = vec4(0.1f, 0.f, 1.f, 1.f);
//			}
//			else if (u_TotalPointLights == 3)
//			{
//				color = vec4(0.f, 0.3f, 1.f, 1.f);
//			}
//			else if (u_TotalPointLights == 4)
//			{
//				color = vec4(0.f, 0.5f, 1.f, 1.f);
//			}
//			else if (u_TotalPointLights == 5)
//			{
//				color = vec4(0.f, 0.5f, 0.5f, 1.f);
//			}
//			else if (u_TotalPointLights == 6)
//			{
//				color = vec4(0.f, 0.5f, 0.1f, 1.f);
//			}
//			else if(u_TotalPointLights == 7)
//			{
//				color = vec4(0.f, 0.5f, 0.f, 1.f);
//			}
//			else if (u_TotalPointLights == 8)
//			{
//				color = vec4(0.1f, 0.7f, 0.f, 1.f);
//			}
//			else if (u_TotalPointLights == 9)
//			{
//				color = vec4(0.4f, 1.f, 0.f, 1.f);
//			}
//			else if (u_TotalPointLights == 10)
//			{
//				color = vec4(0.7f, 1.f, 0.f, 1.f);
//			}
//			else if (u_TotalPointLights == 11)
//			{
//				color = vec4(1.f, 1.f, 0.f, 1.f);
//			}
//			else if (u_TotalPointLights == 12)
//			{
//				color = vec4(1.f, 0.7f, 0.f, 1.f);
//			}
//			else if (u_TotalPointLights == 13)
//			{
//				color = vec4(1.f, 0.4f, 0.f, 1.f);
//			}
//			else if (u_TotalPointLights == 14)
//			{
//				color = vec4(1.f, 0.0f, 0.f, 1.f);
//			}
//			else if (u_TotalPointLights == 15)
//			{
//				color = vec4(1.f, 0.0f, 0.5f, 1.f);
//			}
//			else if (u_TotalPointLights == 16)
//			{
//				color = vec4(1.f, 0.0f, 0.7f, 1.f);
//			}
//			else if (u_TotalPointLights == 17)
//			{
//				color = vec4(1.f, 0.0f, 1.f, 1.f);
//			}
//			else if (u_TotalPointLights == 18)
//			{
//				color = vec4(0.7f, 0.0f, 1.f, 1.f);
//			}
//			else if (u_TotalPointLights == 19)
//			{
//				color = vec4(0.5f, 0.0f, 1.f, 1.f);
//			}
//			else if (u_TotalPointLights == 20)
//			{
//				color = vec4(1.f, 0.0f, 1.f, 1.f);
//			}
//			else if (u_TotalPointLights == 0)
//			{
//				color = vec4(0.f, 0.f, 0.f, 1.f);
//			}
		}
	}
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