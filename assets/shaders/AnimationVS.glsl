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

out vec3 v_FragPosition;
out vec2 v_TextureCoords;
out vec3 v_Normal;
out mat3 v_TBN;

void main()
{
	mat4 jointTransform = skinData[a_Joints.x] * a_Weights.x;
	jointTransform += skinData[a_Joints.y] * a_Weights.y;
	jointTransform += skinData[a_Joints.z] * a_Weights.z;
	jointTransform += skinData[a_Joints.w] * a_Weights.w;
	
	v_FragPosition = vec3(u_Transform * jointTransform * vec4(a_Position,1.0));

	// JOINT TRANSFORM NOT CURRENTLY APPLIED TO NORMAL MAP. SHOULD THEY BE?
	vec4 tempNormal = u_Transform* jointTransform * vec4(a_Normal,1.0);
//
//	vec4 tangentFinal = jointTransform * vec4(a_Tangent,1.0);
//	vec4 bitangentFinal = jointTransform * vec4(a_Bitangent,1.0);

//	v_Normal = mat3(transpose(inverse(u_Transform))) * vec3(tempNormal);		// Oldie but goldie
//
//	vec3 T = normalize(vec3(u_Transform * vec4(tangentFinal.xyz,0.0)));
//	vec3 B = normalize(vec3(u_Transform * vec4(bitangentFinal.xyz, 0.0)));
//	vec3 N = normalize(vec3(u_Transform * vec4(vec3(tempNormal),0.0)));

	v_Normal = mat3(transpose(inverse(u_Transform))) * vec3(a_Normal);		// Oldie but goldie

	vec3 T = normalize(vec3(u_Transform * vec4(a_Tangent, 0.0)));
	vec3 B = normalize(vec3(u_Transform * vec4(a_Bitangent, 0.0)));
	vec3 N = normalize(vec3(u_Transform * vec4(a_Normal,0.0)));
	v_TBN = mat3(T, B, N);

	v_TextureCoords = a_TextureCoords;
	gl_Position = u_ViewProjection * u_Transform * jointTransform * vec4(a_Position,1.0);
}