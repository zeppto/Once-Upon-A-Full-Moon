#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
	gl_Position = vec4(a_Position, 1.0f);
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;
			
uniform vec4 u_Color;

void main()
{
	color = vec4(1.0,1.0,1.0,1.0);
}