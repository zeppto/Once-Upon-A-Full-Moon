#type vertex	
#version 440 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TextureCoords;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TextureCoords;

void main()
{
	v_TextureCoords = a_TextureCoords;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;
			
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_GlossTexture;
uniform sampler2D u_NormalTexture;

in vec2 v_TextureCoords;

void main()
{
	vec4 diffuse = texture(u_DiffuseTexture, v_TextureCoords);
	vec4 gloss = texture(u_GlossTexture, v_TextureCoords);
	vec4 normal = texture(u_NormalTexture, v_TextureCoords);
	color = vec4(diffuse + gloss + normal);
}