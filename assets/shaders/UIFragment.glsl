#version 440 core

layout(location = 0) out vec4 color;

uniform sampler2D u_DiffuseTexture;

in vec2 v_TextureCoords;

void main()
{
	color = texture(u_DiffuseTexture, v_TextureCoords);
}