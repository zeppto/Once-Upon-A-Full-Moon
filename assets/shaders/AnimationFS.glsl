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
	vec3 Position;
	vec3 Direction;
	float Strength;
	mat4 LightSpaceMatrix;
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
layout(binding=10)uniform sampler2D u_ShadowMap; 

uniform vec3 u_CameraPosition;
uniform int u_Shininess;
uniform vec4 u_ObjectColor;
uniform float u_Flash;
uniform int u_RenderShadows;

in vec3 v_FragPosition;
in vec2 v_TextureCoords;
in vec3 v_Normal;
in mat3 v_TBN;
in vec4 v_MVP_Position;

vec3 CalculatePointLight(PointLight light, vec3 normal);
vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 fragPos, vec3 normal, vec3 lightPos);

void main()
{
	// Textures
	vec4 diffuseTexture = texture(u_DiffuseTexture, v_TextureCoords);
	vec4 normalTexture = texture(u_NormalTexture, v_TextureCoords);

	vec3 normal = normalize(normalTexture.rgb * 2.0 - 1.0);
	normal = normalize(v_TBN * normalTexture.rgb);

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
	
	//Shadow
	vec4 fragPosLightSpace = light.LightSpaceMatrix * vec4(v_FragPosition, 1.0);
	float shadow = ShadowCalculation(fragPosLightSpace, v_FragPosition, normal, light.Position);

	return (ambient + (1.0f-shadow)*diffuse);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 fragPos, vec3 normal, vec3 lightPos)
{
float shadow = 0.0;

	if (u_RenderShadows > 0)
	{

		//Perform perspective divition, usually done automatically.
		vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;	//Works both with orthographic projection and perspective projection.

		projCoords = projCoords * 0.5 + 0.5;	//Fixes range from -1,1 to 0,1
		float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
		float currentDepth = projCoords.z;

		//float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
		vec3 lightDir = normalize(lightPos - fragPos);
		vec3 nNormal = normalize(normal);

		//
		//float bias = max(0.00005 * (1.0 - dot(nNormal, lightDir)), 0.000005); //Fixes shadow acne.
	
		//double bias = 0.0001; //Fixes shadow acne.
		double bias = 0.0015; //Fixes shadow acne.

		vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
			}
		}

		shadow /=72;	

		if (projCoords.z > 1.0)
		{
			shadow = 0.0;
		}
	
	//	// perform perspective divide
	//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//    // transform to [0,1] range
	//    projCoords = projCoords * 0.5 + 0.5;
	//    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	//    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
	//    // get depth of current fragment from light's perspective
	//    float currentDepth = projCoords.z;
	//    // check whether current frag pos is in shadow
	//    float shadow = currentDepth > (closestDepth +0.00001)  ? 1.0 : 0.0;

	//   if(closestDepth == currentDepth)
	//	{
	//		color = vec4(1,0,0,1);
	//
	//	}
	//	else
	//	{
	//		color = vec4(0,1,0,1);
	//	}

		//color = vec4(shadow,0,0,1);

	}
	return shadow;
}