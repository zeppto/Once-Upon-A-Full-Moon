#version 440
in vec3 vsColorOut;

layout(location= 4) uniform vec3 color;

out vec4 fsColorOut;

void main()
{
	fsColorOut = vec4(color, 1.0f);
}
