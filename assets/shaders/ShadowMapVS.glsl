#version 440
//VertexShader for depht map
layout(location = 0) in vec3 vsInPos;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() 
{
	gl_Position =  lightSpaceMatrix * model * vec4(vsInPos, 1.0);
}
