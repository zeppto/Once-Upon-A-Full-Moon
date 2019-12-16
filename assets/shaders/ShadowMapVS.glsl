#version 440
//VertexShader for depht map
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TextureCoords;
uniform mat4 u_Transform;
uniform mat4 u_LightSpaceMatrix;
out vec2 v_TextureCoords;

void main() 
{
	v_TextureCoords = a_TextureCoords;
	gl_Position =  u_LightSpaceMatrix * u_Transform * vec4(a_Position, 1.0);
}
