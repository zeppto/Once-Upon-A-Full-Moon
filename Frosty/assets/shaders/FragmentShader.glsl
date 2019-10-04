#version 440 core

in vec3 vsOutPos;
in vec4 vsOutCol;

layout(location = 0) out vec4 fsOutCol;

void main()
{	
	fsOutCol = vsOutCol;	
}
