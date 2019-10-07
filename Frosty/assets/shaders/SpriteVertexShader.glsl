#version 440 core
layout(location = 0) in vec3 vsInPos;
layout(location = 1) in vec4 vsInCol;
layout(location = 2) in vec2 vsInUV;

out vec3 vsOutPos;
out vec4 vsOutCol;
out vec2 vsOutUV;

uniform mat4 model;

void main()
{
	vsOutPos = vsInPos;
	vsOutCol = vsInCol;
	vsOutUV = vsInUV;
	gl_Position = model * vec4(vsInPos, 1.0f);
}