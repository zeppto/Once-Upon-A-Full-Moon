#version 440
//VertexShader for depht map
layout(location = 0) in vec3 vsInPos;

uniform mat4 u_Transform;
uniform mat4 u_LightSpaceMatrix;

void main() 
{
	gl_Position =  u_LightSpaceMatrix * u_Transform * vec4(vsInPos, 1.0);
}
