#version 440 core

in vec3 vsOutPos;
in vec2 vsOutUV;
in vec3 vsOutNorm;

layout(location = 0) out vec4 fsOutCol;
			
void main()
{
	fsOutCol = vec4(0.8f, 0.2f, 0.3f, 1.0f);			
	
	//fsOutCol = vec4(vsOutPos, 1.0f);
	//fsOutCol = vec4(vsOutUV, 0.0f, 1.0f);
	//fsOutCol = vec4(vsOutNorm, 1.0f);
}