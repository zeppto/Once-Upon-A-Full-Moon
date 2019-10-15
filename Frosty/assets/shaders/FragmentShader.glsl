#version 440 core

in vec3 vsOutPos;
in vec4 vsOutCol;

layout(location = 0) out vec4 fsOutCol;
			
void main()
{
	//fsOutCol = vec4(vsOutPos + 0.5f, 1.0f);				
	fsOutCol = vsOutCol;
	fsOutCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}