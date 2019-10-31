#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TextureCoords;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform vec2 u_TextureCoordScale;

out vec3 v_FragPosition;
out vec2 v_TextureCoords;
out vec3 v_Normal;
out mat3 v_TBN;


void main()
{
	v_FragPosition = vec3(u_Transform * vec4(a_Position, 1.0));
	v_TextureCoords = a_TextureCoords;
	//v_TextureCoords = vec2(u_Transform[0][0] * a_TextureCoords.x / u_TextureCoordScale.x, u_Transform[2][2] * a_TextureCoords.y / u_TextureCoordScale.y);
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;		// Oldie but goldie

	vec3 T = normalize(vec3(u_Transform * vec4(a_Tangent,   0.0)));
	vec3 B = normalize(vec3(u_Transform * vec4(a_Bitangent, 0.0)));
	vec3 N = normalize(vec3(u_Transform * vec4(a_Normal,    0.0)));
	v_TBN = mat3(T, B, N);

	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
}