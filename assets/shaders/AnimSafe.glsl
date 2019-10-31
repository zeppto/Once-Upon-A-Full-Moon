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
out vec4 normal;

void main()
{
	mat4 jointTransform = skinData[a_Joints[0]] * a_Weights.x;
	jointTransform += skinData[a_Joints[1]] * a_Weights.y;
	jointTransform += skinData[a_Joints[2]] * a_Weights.z;
	jointTransform += skinData[a_Joints[3]] * a_Weights.w;
	
	gl_Position = u_ViewProjection * u_Transform * (jointTransform * vec4(a_Position,1.0));
	
	v_TextureCoords = a_TextureCoords;
	pos = gl_Position;
	normal = (jointTransform * vec4(a_Normal,1.0));
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;
			
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_GlossTexture;
uniform sampler2D u_NormalTexture;

in vec2 v_TextureCoords;
in vec4 pos;
in vec4 normal;

void main()
{
	vec4 diffuse = texture(u_DiffuseTexture, v_TextureCoords);
	vec4 gloss = texture(u_GlossTexture, v_TextureCoords);
	vec4 normal = texture(u_NormalTexture, v_TextureCoords);
	color = vec4(diffuse + gloss + normal);
}