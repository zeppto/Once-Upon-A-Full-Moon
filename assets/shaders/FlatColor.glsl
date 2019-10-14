#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;
			
uniform vec4 u_ObjectColor;
//uniform vec3 u_LightColor;
//uniform vec3 u_CameraPosition;

void main()
{
	//float ambientStrength = 1.0;
	//vec3 ambient = ambientStrength * u_LightColor;
	//
	//vec3 result = u_LightColor * u_ObjectColor.rgb;
	//
	//color = vec4(result, 1.0f);
	color = u_ObjectColor;
}