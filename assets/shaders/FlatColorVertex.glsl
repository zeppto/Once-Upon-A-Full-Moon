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