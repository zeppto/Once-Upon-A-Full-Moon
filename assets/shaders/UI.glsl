#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TextureCoords;

uniform mat4 u_Transform;

out vec2 v_TextureCoords;

void main()
{
	v_TextureCoords = a_TextureCoords;
	gl_Position = u_Transform * vec4(a_Position, 1.0f);
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;

uniform sampler2D u_DiffuseTexture;

in vec2 v_TextureCoords;

void main()
{
	color = texture(u_DiffuseTexture, v_TextureCoords);
}