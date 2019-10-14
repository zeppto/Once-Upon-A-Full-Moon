#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_Texture;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_FragPosition;
out vec3 v_Normal;

void main()
{
	v_FragPosition = vec3(u_Transform * vec4(a_Position, 1.0));
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;		// Oldie but goldie
	//v_Normal = normalize(vec3(u_Transform) * a_Normal);
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;
			
uniform vec3 u_CameraPosition;
uniform vec4 u_ObjectColor;
uniform vec3 u_LightColor;
uniform vec3 u_LightPosition;
uniform float u_SpecularStrength;
uniform int u_Shininess;
uniform float u_Radius;
uniform float u_Strength;
uniform vec3 u_LightDirection;


in vec3 v_FragPosition;
in vec3 v_Normal;

vec3 CalcPointLight();
vec3 CalcDirLight();

void main()
{
	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * u_LightColor;

	vec3 result;

	// PointLights
	//result += CalcPointLight();

	// DirectionalLights
	result += CalcDirLight();


	result = (ambient + result) * u_ObjectColor.rgb;

	color = vec4(result, 1.0f);
}


vec3 CalcPointLight()
{
	// Diffuse
	vec3 normal = normalize(v_Normal);
	float dist = length(u_LightPosition - v_FragPosition);
	float attenuation = smoothstep(u_Radius * 2, -1, dist);
	vec3 lightDir = normalize(u_LightPosition - v_FragPosition);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * u_LightColor * attenuation * u_Strength;
	
	// Specular
	vec3 viewDir = normalize(u_CameraPosition - v_FragPosition);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
	vec3 specular = u_SpecularStrength * spec * u_LightColor * attenuation;

	return (diffuse + specular);
}

vec3 CalcDirLight()
{
	// Diffuse
	vec3 normal = normalize(v_Normal);
	
	vec3 lightDir = -vec3(mod(u_LightDirection.y, 360.0), mod(u_LightDirection.x, 360) + 1.0, 0.0) / 360.0;
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * u_LightColor;
	
	// Specular
	vec3 viewDir = normalize(u_CameraPosition - v_FragPosition);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
	vec3 specular = u_SpecularStrength * spec * u_LightColor;

	return (diffuse + specular);
}