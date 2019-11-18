#version 440 core

uniform sampler2D u_DiffuseTexture;

in vec2 v_TextureCoords;

out vec4 color;

void main()
{
	color = vec4(texture(u_DiffuseTexture, v_TextureCoords).rgb, 1.0f);
}