#version 440
in vec3 vsColorOut;

out vec4 fsColorOut;

void main()
{
	fsColorOut = vec4(vsColorOut, 1.0f);
}