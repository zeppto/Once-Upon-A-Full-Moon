#version 440 core

layout(location = 0) out vec4 color;
			
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_GlossTexture;
uniform sampler2D u_NormalTexture;

in vec2 v_TextureCoords;
in vec4 pos;
in vec3 normal;
in vec3 colour;
in mat3 TBN;

void main()
{
	vec4 diffuse = texture(u_DiffuseTexture, v_TextureCoords);
	vec4 gloss = texture(u_GlossTexture, v_TextureCoords);
	vec4 normal = texture(u_NormalTexture, v_TextureCoords);
	color = vec4(diffuse + gloss + normal) + vec4(colour,1.0);
}