#type vertex
#version 440 core			
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TextureCoords;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;
layout(location = 5) in vec4 a_Weights;
layout(location = 6) in ivec4 a_Joints;

layout(std140, binding = 7) uniform a_jointDataBlock
{
	mat4 skinData[64];
};

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TextureCoords;
out vec4 pos;
out vec3 normal;
out vec3 colour;
out mat3 TBN;

void main()
{
	mat4 jointTransform = skinData[a_Joints.x] * a_Weights.x;
	jointTransform += skinData[a_Joints.y] * a_Weights.y;
	jointTransform += skinData[a_Joints.z] * a_Weights.z;
	jointTransform += skinData[a_Joints.w] * a_Weights.w;
	
	gl_Position = u_ViewProjection * u_Transform * (jointTransform * vec4(a_Position,1.0));

	normal = mat3(transpose(inverse(u_Transform))) * a_Normal;		// Oldie but goldie

	vec3 T = normalize(vec3(u_Transform * vec4(a_Tangent,   0.0)));
	vec3 B = normalize(vec3(u_Transform * vec4(a_Bitangent, 0.0)));
	vec3 N = normalize(vec3(u_Transform * vec4(a_Normal,    0.0)));
	TBN = mat3(T, B, N);

	colour = vec3(a_Weights.y,a_Weights.w,a_Weights.z);
	v_TextureCoords = a_TextureCoords;
	pos = gl_Position;
}

#type fragment
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